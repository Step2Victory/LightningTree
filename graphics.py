import plotly.graph_objects as go
import pandas as pd
import math

from plotly.subplots import make_subplots
from jupyter_dash import JupyterDash
from dash import dcc, html, Output, Input

# from dash.dependencies import Output, Input

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
        
        fig.update_layout(uirevision=True)
        return fig


    def plot_two(self, df_q, df_Q):
        data = [go.Scatter(x=df_q.values[::-1].ravel(), y=df_q.index.values[::-1], 
                           mode='lines+markers', name='sum' + ' q'),
                go.Scatter(x=df_Q.values[::-1].ravel(), y=df_Q.index.values[::-1], 
                           mode='lines+markers', name='sum' + ' Q')]
        fig = go.Figure(data=data, layout={'uirevision': 'True'})
        return fig

    def plot_tree(self) -> go.Figure:
        """
        Создание 3D графа дерева молнии
        
        Parametrs
        ---------
        return: Графический объект
        """
        # Настройки для отображения графиков
        scale_nodes = [(0, "darkblue"), (0.15, "blue"), (0.49, "yellow"), (0.5, "gray"), (0.51, "yellow"), (0.85, "red"), (1, "darkred")] # цветовая шкала для зарядов
        scale_case = [(0, "darkblue"), (0.15, "blue"), (0.49, "yellow"), (0.5, "white"), (0.51, "yellow"), (0.85, "red"), (1, "darkred")] # цветовая шкала для чехлов
        setting = {'showbackground': False, 'showticklabels': False, 'showgrid': False, 'zeroline': False} # Параметры отображения системы координат
        setting_z = {'showbackground': True, 'showticklabels': True, 'showgrid': False, 'zeroline': False} # Параметры отображения системы координат для оси z

        # Создание настройки отображения графического объекта graph_object
        layout = go.Layout(showlegend=False, hovermode='closest',
                       scene={'xaxis': setting, 'yaxis': setting, 'zaxis': setting_z},
                       uirevision=True)
        
        # Набор DataFrame'а для создания графа
        array = []
        for index, row in self.df_edge.iterrows():
            array.append(row['from'])
            array.append(row['to'])
            array.append(None)
        df_edges = pd.DataFrame({'id' : array}).merge(self.df_vertex[['id', 'x', 'y', 'z']], on='id', how='left')

        # Построение рёбер
        edge_trace = go.Scatter3d(x=df_edges.x, y=df_edges.y, z=df_edges.z, 
                                  line=dict(width=2, color="darkblue"),
                                  hoverinfo='none',
                                  mode='lines')
        
        # Построение зарядов    
        node_trace = go.Scatter3d(x=self.df_vertex.x, y=self.df_vertex.y, z=self.df_vertex.z,
                                  mode='markers',
                                  marker=dict(showscale=True, colorscale=scale_nodes, color=self.df_vertex.q, cmin=-0.001, cmax=0.001, size=2.4),
                                  line_width=.1)

        # Построение чехлов
        case_trace = go.Scatter3d(x=self.df_vertex.x, y=self.df_vertex.y, z=self.df_vertex.z,
                                  mode='markers',
                                  marker=dict(showscale=False, colorscale=scale_case, color=self.df_vertex.Q, cmin=-0.000001, cmax=0.000001, size=12),
                                  text = self.df_vertex.q,
                                  customdata= self.df_vertex.Q,
                                  hovertemplate='q= %{text} <br>Q= %{customdata}<extra></extra>',
                                  line_width=1,
                                  opacity=0.1)
        
        data = [edge_trace, node_trace, case_trace]
        result = go.Figure(data=data, layout=layout)
        return result
    

    def run(self, mode:str='external', interval:int=0):
        """
        Метод для запуска Dash-приложения

        Parametrs
        ---------
        mode: Параметр запуска (inline - внутри jupyter; external - в браузере)
        interval: интервал обновления в секундах
        """
        # Создание Dash-приложения
        app = JupyterDash('SimpleExemple')

        # Настройка и запуск Dash-приложения

        # app.layout = html.Div([html.H1("Модель молнии", style={'textAlign': 'center', 'color': 'gold'}),
                            #    html.Div([html.H4("Распределение заряда по высоте", style={'textAlign': 'center'}),
                            #              dcc.Graph(figure=(self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
                            #                                           self.distribution(self.df_vertex, 'z', 'sum', 'Q')])),
                            #                        style={'height': '90vh'})],
                            #            style={'display': 'inline-block', 'width': '40%'}),
                            #    html.Div([html.H4("Граф дерева", style={'textAlign': 'center'}),
                            #             dcc.Graph(figure=(self.plot_tree()), style={'height': '90vh'})],
                            #            style={'display': 'inline-block', 'width': '60%'})])
        
        # app.layout = html.Div([html.H1("Модель молнии", style={'textAlign': 'center', 'color': 'gold'}),
                            #    html.Div([html.H4("Распределение заряда по высоте", style={'textAlign': 'center'}),
                            #              dcc.Graph(figure=(self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
                            #                                           self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                            #                                           self.fi_def(self.df_vertex)])),
                            #                        style={'height': '90vh'})],
                            #            style={'display': 'inline-block', 'width': '40%'}),
                            #    html.Div([html.H4("Граф дерева", style={'textAlign': 'center'}),
                            #             dcc.Graph(figure=(self.plot_tree()), style={'height': '90vh'})],
                            #            style={'display': 'inline-block', 'width': '60%'})])
        if interval == 0:
            app.layout = html.Div([html.H1("Модель молнии", style={'textAlign': 'center', 'color': 'gold'}),
                                    
                                    html.Div([html.H4("Распределение заряда по высоте", style={'textAlign': 'center'}),
                                                
                                                dcc.Dropdown(options=[{'label':"Распределение суммы зарядов по высоте", 'value':'sum'}, 
                                                            {'label': "Распределение среднего значения зарядов по высоте", 'value':'avg'},
                                                       {'label': "Распределение полного потенциала по высоте", 'value':'full_phi'},
                                                       {'label': "Распределение потенциала внешнего поля по высоте", 'value':'ext_phi'},
                                                       {'label': "Распределение токов по высоте", "value" : "current"},
                                                       {'label': "Все графики", 'value':'all'}, {'label': "По умолчанию", 'value':'default'}], value='default', id='dropdown'),

                                                dcc.Graph(id='graph_distrib',
                                                    #    figure=(self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
                                                    #                       self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                                                    #                       self.fi_def(self.df_vertex)])),
                                                        style={'height': '90vh'})],
                                                        style={'display': 'inline-block', 'width': '39%'}),

                                    html.Div([html.H4("Граф дерева", style={'textAlign': 'center'}),
                                                
                                            dcc.Graph(figure=(self.plot_tree()), id='graph_tree', style={'height': '90vh'})],

                                            style={'display': 'inline-block', 'width': '59%'})])
        else:
            app.layout = html.Div([html.H1("Модель молнии", style={'textAlign': 'center', 'color': 'gold'}),
                               
                               dcc.Interval(id='interval-component', interval=interval*1000, n_intervals=0),

                               html.Div([html.H4("Распределение заряда по высоте", style={'textAlign': 'center'}),
                                         
                                         dcc.Dropdown(options=[{'label':"Распределение суммы зарядов по высоте", 'value':'sum'}, 
                                                       {'label': "Распределение среднего значения зарядов по высоте", 'value':'avg'},
                                                       {'label': "Распределение полного потенциала по высоте", 'value':'full_phi'},
                                                       {'label': "Распределение потенциала внешнего поля по высоте", 'value':'ext_phi'},
                                                       {'label': "Распределение токов по высоте", "value" : "current"},
                                                       {'label': "Все графики", 'value':'all'}, {'label': "По умолчанию", 'value':'default'}], value='default', id='dropdown'),

                                         dcc.Graph(id='graph_distrib',
                                                #    figure=(self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
                                                #                       self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                                                #                       self.fi_def(self.df_vertex)])),

                                                   style={'height': '90vh'})],
                                                   style={'display': 'inline-block', 'width': '39%'}),

                               html.Div([html.H4("Граф дерева", style={'textAlign': 'center'}),
                                         
                                        dcc.Graph(figure=(self.plot_tree()), id='graph_tree', style={'height': '90vh'})],

                                        style={'display': 'inline-block', 'width': '59%'})])

    


        figures = {
            "sum" : self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
                                self.distribution(self.df_vertex, 'z', 'sum', 'Q')]),
            "avg" : self.plot([self.distribution(self.df_vertex, 'z', 'mean', 'q'), 
                                     self.distribution(self.df_vertex, 'z', 'mean', 'Q')]),
            'full_phi' : self.plot([self.distribution(self.df_phi_info, 'z', 'mean', 'full_phi')]),
            'ext_phi' : self.plot([self.distribution(self.df_phi_info, 'z', 'mean', 'ext_phi')]),
            
            'current' : self.plot([self.distribution(self.df_edge, 'z', 'mean', 'current')]),
            'all' : self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'),
                                           self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                                           self.distribution(self.df_vertex, 'z', 'mean', 'q'), 
                                           self.distribution(self.df_vertex, 'z', 'mean', 'Q'),
                                           self.distribution(self.df_vertex, 'z', 'mean', 'phi')]),
            'default' : self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'),
                                   self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                                   self.distribution(self.df_vertex, 'z', 'mean', 'phi'),
                                   self.fi_def(self.df_vertex)])
            
        }
        
        @app.callback(Output('graph_distrib', 'figure'),
                      [Input('dropdown', 'value')])
        def update_figure(value):
            # match value:
                # case 'sum':
                #     figure = self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
                #                      self.distribution(self.df_vertex, 'z', 'sum', 'Q')])
                # case 'avg':
                #     figure = self.plot([self.distribution(self.df_vertex, 'z', 'mean', 'q'), 
                #                      self.distribution(self.df_vertex, 'z', 'mean', 'Q')])
                # case 'fi':
                #     figure = self.plot([self.fi_def(self.df_vertex)])
                # case 'all':
                #     figure = self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'),
                #                            self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                #                            self.distribution(self.df_vertex, 'z', 'mean', 'q'), 
                #                            self.distribution(self.df_vertex, 'z', 'mean', 'Q'),
                #                            self.fi_def(self.df_vertex)])
                # case _:
                #     figure = self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'),
                #                            self.distribution(self.df_vertex, 'z', 'sum', 'Q'),
                #                            self.fi_def(self.df_vertex)])

            return figures[value]
        

        @app.callback(Output('graph_tree', 'figure'),
                      Input('interval-component', 'n_intervals'))
        def update_graph_live(n):

            self.df_vertex = self.open_file(self.folder + '/vertex_table.txt')
            self.df_edge = self.open_file(self.folder + '/edge_table.txt')

            fig = self.plot_tree()

            return fig
        
        app.run_server(mode=mode)


def main():
    lt = LightningTree("LightningTree_data")
    lt.run()
    # print(lt.distribution(lt.df_vertex, 'z', 'sum', 'q'))
    # print(lt.fi_def(lt.df_vertex))
    # print(lt.df_vertex.z.sort_values()[::-1].unique())
    


if __name__ == '__main__':
    main()
    