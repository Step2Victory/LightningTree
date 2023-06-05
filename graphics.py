import plotly.graph_objects as go
import plotly.io as pio
import pandas as pd
import math, io
# import time, multiprocessing as mps

from plotly.subplots import make_subplots
from jupyter_dash import JupyterDash
from dash import dcc, html, Output, Input, State, ctx
from PIL import Image

eps0 = 8.85418781281313131313e-12
k = 1 / (4 * math.pi * eps0)

class Vector(object):
    def __init__(self, _point: list[float]):
        self.x = _point[0]
        self.y = _point[1]
        self.z = _point[2]

    def __str__(self):
        return "({}, {}, {})".format(self.x, self.y, self.z)

    def __iter__(self):
        return self

    def __next__(self):
        return self
    
    def radius(self):
        return (self.x**2 + self.y**2 + self.z**2)**0.5

class LightningTree(object):
    figure_tree = go.Figure()
    figure_plots = {}

    def __init__ (self, folder):
        self.folder = folder
        self.df_vertex = self.open_file(folder+'/vertex_table.txt')
        self.df_edge = self.open_file(folder+'/edge_table.txt')
        self.df_phi_info = self.open_file(folder+'/phi_info.txt')

        def get_middle_edge(row):
            """
            Возвращает середину ребра
            
            Parametrs
            ---------
            row: Строка из датафрейма ребер
            return: середину ребра, заданного в строке row
            """
            from_id = row['from']
            to_id = row['to']
            return (self.df_vertex[self.df_vertex['id'] == from_id]['z'].item() + self.df_vertex[self.df_vertex['id'] == to_id]['z'].item()) / 2
        
        self.df_edge['z'] = self.df_edge.apply(get_middle_edge, axis=1)
        # self.df_q_history = self.open_file(folder+'/q_history_1.txt')
        # self.df_Q_history = self.open_file(folder+'/Q_history.txt')


    def outImages(self, folder:str, format:str, i:int = -1):
        if i != -1:
            pio.write_image(self.figure_tree, folder + "/tree_%s.%s" % (i, format), format)
            for plot in self.figure_plots:
                pio.write_image(self.figure_plots[plot], folder + "/%s_%s.%s" % (plot, i, format), format)
        else:
            pio.write_image(self.figure_tree, folder + "/tree.%s" % format, format)
            for plot in self.figure_plots:
                pio.write_image(self.figure_plots[plot], folder + "/%s.%s" % (plot, format), format)


    def __str__(self):
        return "({}, {})".format(self.figure_tree, self.figure_plots)


    def open_file(self, filename: str) -> pd.DataFrame:
        """
        Метод чтения данных из файла
        
        Parametrs
        ---------
        filename: Имя файла для чтения. Если файл находиться не в проекте, то указывается полное имя файла
        return: Данные файла
        """
        result = pd.DataFrame()
        if "history" in filename:
            with open(filename, 'r') as file:
                for line in file:
                    array = (line.rstrip()).split(" ")
                    temp = pd.DataFrame({array[0] : array[1:]})
                    result = pd.concat([result, temp], axis=1)
                    result = result.astype('float')
                    
        else:
            result = pd.read_csv(filename, delim_whitespace=True)
        return result


    def distribution(self, df:pd.DataFrame, groupby:str, operarion:str, colomn:str) -> pd.DataFrame:
        """
        Метод расчета значений с группировкой данных по столбцу

        Parametrs
        ---------
        df: Данные для обработки
        groupby: Заголовок столбца по которому будет происходит группировка
        operation: Групповая операция по данным (sum, min, max, mean, ...)
        colomn: Заголовок столбца для которого будет осуществляться операция
        return: Сортированные по столбцу данные с распределением значений.
        """
        # return df.groupby([groupby]).agg({colomn:[operarion]})
        result = df.groupby([groupby]).agg({colomn:[operarion]})
        return pd.DataFrame({groupby : result.index.values[::-1], operarion+' '+colomn : result.values[::-1].ravel()})


    def fi_def(self, df:pd.DataFrame) -> pd.DataFrame:
        """
        Метод расчёта потенциала по столбцу
        
        Parametrs
        ---------
        df: Данные для обработки
        charge: заряд в вершине графа или в чехле ('q', 'Q')
        return: Сортированные по высоте данные с распределением потенциала
        """
        fi_list = []
        step = (int) ((df.z.max() - df.z.min()) / (df.nunique().z -1))
        for h in range(df.z.min(), df.z.max() + step, step):
            fi = 0
            for index, row in df.iterrows():
                if row.y == 0 and row.x == 0 and h == row.z:
                    fi += k * (row['q'] + row['Q']) * (1 / (Vector([0-row.x, 0 - row.y, h-row.z]).radius()+step/2)
                                        - 1 / (Vector([0-row.x, 0 - row.y, h+row.z]).radius()+step/2))
                else:
                    fi += k * (row['q'] + row['Q']) * (1 / (Vector([0-row.x, 0 - row.y, h-row.z]).radius())
                                        - 1 / (Vector([0-row.x, 0 - row.y, h+row.z]).radius()))
            fi_list.append([h, fi])
        
        return pd.DataFrame(fi_list, columns=['z', 'fi'])
        # fi = pd.DataFrame()          
        # for i, row in df.iterrows():
        #     fi[i] = df['z'].apply(lambda h: k * row[charge] * (1 / (Vector([0-row.x, 0 - row.y, h-row.z]).radius()+5) - 
        #                                                            1 / (Vector([0-row.x, 0 - row.y, h+row.z]).radius()+5))
        #                                         if row.y == 0 and row.x == 0 and h == row.z else
        #                                         k * row[charge] * (1 / (Vector([0-row.x, 0 - row.y, h-row.z]).radius()) - 
        #                                                            1 / (Vector([0-row.x, 0 - row.y, h+row.z]).radius()))
        #                             )
        # result = pd.DataFrame(df.z)
        # result['fi'] = fi.sum(axis=1)
        # return self.distribution(result,'z', 'mean', 'fi')


    def plot(self, list_df:list[pd.DataFrame]) -> go.Figure:
        """
        Создание 2D графика
        
        Parametrs
        ---------
        list_df: Лист данных для построения графика. (индекс - у, колонка - x)
        return: Графический объект
        """
        fig = make_subplots(rows=1, cols=len(list_df), shared_yaxes=True, horizontal_spacing=0.02)
        for i in range(len(list_df)):
            fig.add_trace(go.Scatter(x=list_df[i][list_df[i].columns[1]],
                                    y=list_df[i][list_df[i].columns[0]], 
                                    mode='lines+markers', 
                                    name=list_df[i].columns[1]), 
                            row=1, col=i+1)
        
        fig.update_layout(uirevision=True, yaxis={'range':[7000,11000]})
        return fig


    # def plot_two(self, df_q, df_Q):
        # data = [go.Scatter(x=df_q.values[::-1].ravel(), y=df_q.index.values[::-1], 
        #                    mode='lines+markers', name='sum' + ' q'),
        #         go.Scatter(x=df_Q.values[::-1].ravel(), y=df_Q.index.values[::-1], 
        #                    mode='lines+markers', name='sum' + ' Q')]
        # fig = go.Figure(data=data, layout={'uirevision': 'True'})
        # return fig
    

    def plots(self):
        """
        Создания словаря графических объектов из 2D графиков
        """
        self.figure_plots = {
            "sum_q" : self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
                                self.distribution(self.df_vertex, 'z', 'sum', 'Q')]),
            "avg_q" : self.plot([self.distribution(self.df_vertex, 'z', 'mean', 'q'),
                                     self.distribution(self.df_vertex, 'z', 'mean', 'Q')]),
            'Phi' : self.plot([self.distribution(self.df_phi_info, 'z', 'mean', 'full_phi'),
                               self.distribution(self.df_phi_info, 'z', 'mean', 'ext_phi')]),
            'current' : self.plot([self.distribution(self.df_edge, 'z', 'mean', 'current')]),
            'default' : self.plot([self.distribution(self.df_edge, 'z', 'mean', 'current'),
                                   self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                                   self.distribution(self.df_edge, 'z', 'mean', 'sigma'),
                                   self.fi_def(self.df_vertex)
                                   ])
            
        }

    def plot_tree(self):# -> go.Figure:
        """
        Создание графического объекта 3D графа дерева молнии
        """
        # Настройки для отображения графиков
        scale_nodes = [(0, "darkblue"), (0.15, "blue"), (0.49, "yellow"), (0.5, "gray"), (0.51, "yellow"), (0.85, "red"), (1, "darkred")] # цветовая шкала для зарядов
        scale_case = [(0, "darkblue"), (0.15, "blue"), (0.49, "yellow"), (0.5, "white"), (0.51, "yellow"), (0.85, "red"), (1, "darkred")] # цветовая шкала для чехлов
        setting = {'showbackground': False, 'showticklabels': True, 'showgrid': False, 'zeroline': False, 'range':[-500, 500]} # Параметры отображения системы координат
        setting_z = {'showbackground': True, 'showticklabels': True, 'showgrid': False, 'zeroline': False, 'range':[7000,11000]} # Параметры отображения системы координат для оси z

        # Создание настройки отображения графического объекта graph_object
        layout = go.Layout(showlegend=False, hovermode='closest',
                       scene={'xaxis': setting, 'yaxis': setting, 'zaxis': setting_z},
                       uirevision=True)
        
        # Набор DataFrame'а для создания графа
        array = []
        for index, row in self.df_edge.iterrows():
            array.append([row['from'], row.current])
            array.append([row['to'], row.current])
            array.append([None, None])
        df_edges = pd.DataFrame(array, columns=['id', 'current']).merge(self.df_vertex[['id', 'x', 'y', 'z']], on='id', how='left')
        # print(df_edges[df_edges.current.notna()].current.unique())
        
        # Построение рёбер
        edge_trace = go.Scatter3d(x=df_edges.x, y=df_edges.y, z=df_edges.z, 
                                  line=dict(width=2, color=df_edges.current, colorscale=["darkslateblue", "crimson"], cmin=-1, cmax=1),
                                  text=df_edges.current,
                                  hovertemplate='I=%{text}',
                                #   hoverinfo='none',
                                  mode='lines')
        
        # Построение зарядов    
        node_trace = go.Scatter3d(x=self.df_vertex.x, y=self.df_vertex.y, z=self.df_vertex.z,
                                  mode='markers',
                                  marker=dict(showscale=True, colorscale=scale_nodes, color=self.df_vertex.q, cmin=-0.001, cmax=0.001, size=2.4),
                                  line_width=.1)

        # Построение чехлов
        case_trace = go.Scatter3d(x=self.df_vertex.x, y=self.df_vertex.y, z=self.df_vertex.z,
                                  mode='markers',
                                  marker=dict(showscale=False, colorscale=scale_case, color=self.df_vertex.Q, cmin=-0.1, cmax=0.1, size=12),
                                  text = self.df_vertex.q,
                                  customdata= self.df_vertex.Q,
                                  hovertemplate='q= %{text} <br>Q= %{customdata}<extra></extra>',
                                  line_width=1,
                                  opacity=0.1)
        
        data = [edge_trace, node_trace, case_trace]
        self.figure_tree = go.Figure(data=data, layout=layout)
        # result = go.Figure(data=data, layout=layout)
        # return result

#def outImageTree(lt:LightningTree, i:int, folder:str, format:str):
#    lt.plot_tree()
#    pio.write_image(lt.figure_tree, folder + "/tree_%s.png" % (i), format)

#def outImagePlots(lt:LightningTree, i:int, folder:str, format:str):
#    lt.plots()
#    for plot in lt.figure_plots:
#        pio.write_image(lt.figure_plots[plot], folder + "/%s_%s.png" % (plot, i), format)

#def out_images(lt_history:list[LightningTree], folder:str, format:str):
#    i = 0
#    for lt in lt_history:
#        lt.outImages(folder, format, i)
#        i+=1

def out_animations(lt_history:list[LightningTree], folder:str, format:str='jpg'):
    print('Start export animations')
    set_images = []
    for lt in lt_history:
        pic_defPlot = Image.open(io.BytesIO(pio.to_image(lt.figure_plots['default'], format)))
        w, h = pic_defPlot.size

        pic_PhiPlot = Image.open(io.BytesIO(pio.to_image(lt.figure_plots['Phi'], format)))
        pic_tree = Image.open(io.BytesIO(pio.to_image(lt.figure_tree, 'png', w*2, h*2)))     
        W, H = pic_tree.size

        result = Image.new(pic_defPlot.mode, (w + W, H), color=0)
        
        result.paste(pic_defPlot, box=(0, 0))
        result.paste(pic_PhiPlot, box=(0, h))
        result.paste(pic_tree, box=(w, 0))

        set_images.append(result)
            
    set_images[0].save(
        folder + "/LightningTree.gif",
        save_all=True,
        append_images=set_images[1:],
        optimize=True,
        duration=200,
        loop=0)
    print('Finish')


def run(folder:str, mode:str='external', interval:int=0):
    """
    Метод для запуска Dash-приложения

    Parametrs
    ---------
    mode: Параметр запуска (inline - внутри jupyter; external - в браузере)
    interval: интервал обновления в секундах
    """
    # Создание Dash-приложения
    app = JupyterDash('SimpleExemple')
    disable = False

    # start_time = time.time()

    lt_history = [LightningTree(folder)]
    lt_history[0].plot_tree()   # вытащить в отдельный процесс(поток)
    # process = mps.Process(target=lt_history[0].plot_tree(), args=())
    # process.start()
    lt_history[0].plots()
    # process.join()
    # process.close()
    
    # print("--- %s seconds ---" % (time.time() - start_time))
    
    if interval == 0:
        disable = True

    # Настройка и запуск Dash-приложения в зависимости от параметра
    app.layout = html.Div([html.H1("Модель молнии", style={'textAlign': 'center', 'color': 'gold'}),
                        
                        dcc.Interval(id='interval-component', interval=interval*1000, n_intervals=0, disabled=disable),# max_intervals=20),
                        # html.Div(html.Button('Out images', id='out_images_button', n_clicks=0, disabled = not disable)),

                        html.Div([html.H4("Распределение заряда по высоте", style={'textAlign': 'center'}),
                                    dcc.Dropdown(options=[{'label':"Распределение суммы зарядов по высоте", 'value':'sum_q'}, 
                                                {'label': "Распределение среднего значения зарядов по высоте", 'value':'avg_q'},
                                                {'label': "Распределение потенциала по высоте", 'value':'Phi'},
                                                {'label': "Распределение токов по высоте", "value" : "current"},
                                                {'label': "По умолчанию", 'value':'default'}], value='default', id='dropdown'),
                                    dcc.Graph(#figure=lt_history[-1].figure_plots['default'], 
                                              id='graph_distrib', style={'height': '80vh'})],
                                    style={'display': 'inline-block', 'width': '39%'}),

                        html.Div([html.H4("Граф дерева", style={'textAlign': 'center'}),
                                    dcc.Graph(figure=lt_history[-1].figure_tree, id='graph_tree', style={'height': '80vh'})],
                                    style={'display': 'inline-block', 'width': '59%'}),

                        html.Div(dcc.Slider(0, 1, step = 1, id='time_slider', disabled=disable),
                                 style={'display': 'inline-block', 'width': '95%'}),

                        html.Div([html.Button('Start/Stop', id='stop_button', n_clicks=0, disabled=disable),

                                 html.Button('Out images', id='out_images_button', n_clicks=0, disabled=not disable)],
                                 
                                 style={'display': 'inline-block', 'width': '3%'})
                        ])
    

    @app.callback(Output('graph_tree', 'figure'),
                  Output('time_slider', 'max'),
                  Input('interval-component', 'n_intervals'),
                  Input('time_slider', 'value'))
    def update_graph_live(n, t):
        # if n is None:
        #     raise JupyterDash.exceptions.PreventUpdate
        time = -1
        if ctx.triggered_id == 'interval-component':
            lt_history.append(LightningTree(folder))
            lt_history[-1].plot_tree() # вытащить в отдельный процесс (поток)
            lt_history[-1].plots()

        if ctx.triggered_id == 'time_slider':
            time = t

        return lt_history[time].figure_tree, len(lt_history)-1
    

    @app.callback(Output('graph_distrib', 'figure'),
                  Input('dropdown', 'value'),
                  Input('interval-component', 'n_intervals'),
                  Input('time_slider', 'value'))
    def update_figure(value, n, t):
        time = -1

        if ctx.triggered_id == 'time_slider':
            time = t
        
        return lt_history[time].figure_plots[value]
    

    @app.callback(Output('interval-component', 'disabled'),
                  Output('out_images_button', 'disabled'),
                  Input('stop_button', 'n_clicks'),
                  State('interval-component', 'disabled'))
    def toggle_interval(click, disabled):
        if click:
            return not disabled, disabled
        return disabled, not disabled
    
    
    @app.callback(Output('stop_button', 'disabled'),
                  Input('out_images_button', 'n_clicks'),
                  State('interval-component', 'disabled'))
    def export_images(clicks, disabled):
        if clicks and disabled:
            out_animations(lt_history, "LightningTree_data/Animations", 'jpg')
        # if clicks%5 == 0:
        #     return not disabled
        return False
    
    
    app.run_server(mode=mode)


def create_gif(folder:str, names:str|list[str], format:str='jpg', start:int=0, end:int=10):
    # Список для хранения кадров.
    def work(name):
        frames = [Image.open(folder + "/Images/{}_{}.{}".format(name, start, format))]
        # drtn = [1000]
        # print(frames[0])
        for i in range(start, end):
            # Открываем изображение каждого кадра.
            frame = Image.open(folder + "/Images/{}_{}.{}".format(name, i, format))
            # Добавляем кадр в список с кадрами.
            # if frame != frames[i-1]:
            frames.append(frame)
            # step = 1000/float(i-start+1)
            # if step <= 100:
            #     drtn.append(100)
            # else:
            #     drtn.append(int(step))

        # Берем первый кадр и в него добавляем оставшееся кадры.
        frames[0].save(
            folder + f"/Animations/{name}.gif",
            save_all=True,
            append_images=frames[1:],  # Срез который игнорирует первый кадр.
            optimize=True,
            duration=200,
            loop=0
        )

    if type(names) is str:
        work(names)
    else:
        for n in names:
            work(n)

# def out_animations(lt_history:list[LightningTree], folder:str):
#     gif_tree = []
#     gif_plots = {'default':[]}
#     for lt in lt_history:
#         image_tree = pio.to_image(lt.figure_tree, 'jpg')
#         gif_tree.append(Image.open(image_tree))
#         for key, plot in lt.figure_plots:
#             gif_plots[key].append(Image.open(pio.to_image(plot, 'jpg')))
    
#     # gif_tree[0].save(
#     #     folder + "/tree.gif",
#     #     save_all=True,
#     #     appen_images=gif_tree[1:],
#     #     optimize=True,
#     #     duration=200,
#     #     loop=0
#     # )
#     # for key, plot in gif_plots:
#     #     plot[0].save(
#     #         folder + f"/{key}.gif",
#     #         save_all=True,
#     #         appen_images=plot[1:],
#     #         optimize=True,
#     #         duration=200,
#     #         loop=0
#     #     )


def main():
    run("LightningTree_data", interval=1)

    # lt.run()#interval=1)
    # print(lt.distribution(lt.df_vertex, 'z', 'sum', 'q'))
    # print(lt.fi_def(lt.df_vertex))
    # print(lt.df_vertex.z.sort_values()[::-1].unique())

    # start_time = time.time()
    # lt = [LightningTree("LightningTree_data")]
    # lt[0].plot_tree()
    # lt[0].plots()
    # pio.write_image(lt[0].figure_tree, "LightningTree_data/Images/tree_%s.png" % (1), 'png')
    # for plot in lt[0].figure_plots:
    #     pio.write_image(lt[0].figure_plots[plot], "LightningTree_data/Images/%s_%s.png" % (plot, 1), 'png')
    # image_tree = [pio.to_image(lt[0].figure_tree, 'png')]
    # funcion(lt[0], 1, "LightningTree_data/Images", 'png')
    # funcion2(lt[0], 1, "LightningTree_data/Images", 'png')
    # print("--- %s seconds ---" % (time.time() - start_time))

    # create_gif("LightningTree_data", ["tree", "all", "default", "avg", "current", "ext_phi", "full_phi", "sum"], 'jpg', start=10, end=178)



if __name__ == '__main__':
    main()
    