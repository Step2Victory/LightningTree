import plotly.graph_objects as go
import pandas as pd
import math

from plotly.subplots import make_subplots
from jupyter_dash import JupyterDash
from dash import dcc, html

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
        self.df_vertex = self.open_file(folder+'/vertex_table.txt')
        self.df_edge = self.open_file(folder+'/edge_table.txt')
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
        return: Данные с одной колонкой.
        """
        # Приведение в нормальный вид с 2я колонками???
        return df.groupby([groupby]).agg({colomn:[operarion]})
    

    def fi_def(self, df:pd.DataFrame, charge:str) -> pd.DataFrame:
        """
        Метод расчёта потенциала по столбцу
        
        Parametrs
        ---------
        df: Данные для обработки
        charge: заряд в вершине графа или в чехле ('q', 'Q')
        return: Распределение заряда по высоте
        """
        # fi = {}
        # for height in range(df.z.min(), df.z.max(), 10):
        #     for i, row in df.iterrows():
        #         if row.y == 0 and row.x == 0 and height == row.z:
        #             fi[height] += k * row[charge] * (1 / (((0 - row.x) ** 2 +
        #                                         (0 - row.y) ** 2 +
        #                                         (height - 0.001 - row.z) ** 2) ** 0.5)
        #                                 - 1 / (((0 - row.x) ** 2 +
        #                                         (0 - row.y) ** 2 +
        #                                         (height - 0.001 + row.z) ** 2) ** 0.5))
        #         else:
        #             fi[height] += k * row[charge] * (1 / (((0 - row.x) ** 2 +
        #                                         (0 - row.y) ** 2 +
        #                                         (height - row.z) ** 2) ** 0.5)
        #                                 - 1 / (((0 - row.x) ** 2 +
        #                                         (0 - row.y) ** 2 +
        #                                         (height + row.z) ** 2) ** 0.5))
        fi = pd.DataFrame()          
        for i, row in df.iterrows():
            fi[i] = df['z'].apply(lambda h: k * row[charge] * (1 / (Vector([0-row.x, 0 - row.y, h-0.001-row.z]).radius()) - 
                                                                   1 / (Vector([0-row.x, 0 - row.y, h-0.001+row.z]).radius()))
                                                if row.y == 0 and row.x == 0 and h == row.z else
                                                k * row[charge] * (1 / (Vector([0-row.x, 0 - row.y, h-row.z]).radius()) - 
                                                                   1 / (Vector([0-row.x, 0 - row.y, h+row.z]).radius()))
                                    )
        result = pd.DataFrame(df.z)
        result['fi'] = fi.sum(axis=1)
        return self.distribution(result,'z', 'mean', 'fi')


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
            fig.add_trace(go.Scatter(x=list_df[i].values[::-1].ravel(),
                                    y=list_df[i].index.values[::-1], 
                                    mode='lines+markers', 
                                    name=list_df[i].columns.values[0][1] + ' ' + list_df[i].columns.values[0][0]), 
                            row=1, col=i+1)
        
        fig.update_layout(uirevision=True)
        return fig


    def plot_tree(self) -> go.Figure:
        """
        Создание 3D графа дерева молнии

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
    

    def run(self):
        """
        Метод для запуска Dash-приложения
        """
        # Создание Dash-приложения
        app = JupyterDash('SimpleExemple')

        # Настройка и запуск Dash-приложения
        # app.layout = html.Div([html.H1("Модель молнии", style={'textAlign': 'center', 'color': 'gold'}),
        #                        html.Div([html.H4("Распределение заряда по высоте", style={'textAlign': 'center'}),
        #                                  dcc.Graph(figure=(self.plot([self.distribution(self.df_vertex, 'z', 'sum', 'q'), 
        #                                                               self.distribution(self.df_vertex, 'z', 'sum', 'Q')])),
        #                                            style={'height': '90vh'})],
        #                                style={'display': 'inline-block', 'width': '30%'}),
        #                        html.Div([html.H4("Граф дерева", style={'textAlign': 'center'}),
        #                                 dcc.Graph(figure=(self.plot_tree()), style={'height': '90vh'})],
        #                                style={'display': 'inline-block', 'width': '70%'})])
        
        app.layout = html.Div([html.H1("Модель молнии", style={'textAlign': 'center', 'color': 'gold'}),
                               html.Div([html.H4("Распределение заряда по высоте", style={'textAlign': 'center'}),
                                         dcc.Graph(figure=(self.plot([self.fi_def(self.df_vertex, 'q'), 
                                                                      self.fi_def(self.df_vertex, 'Q')])),
                                                   style={'height': '90vh'})],
                                       style={'display': 'inline-block', 'width': '30%'}),
                               html.Div([html.H4("Граф дерева", style={'textAlign': 'center'}),
                                        dcc.Graph(figure=(self.plot_tree()), style={'height': '90vh'})],
                                       style={'display': 'inline-block', 'width': '70%'})])

        app.run_server(mode='external')  # inline - внутри jupyter; external - в браузере


def main():
    lt = LightningTree("LightningTree_data")
    lt.run()
    # print(lt.distribution(lt.df_vertex, 'z', 'sum', 'q').columns.values[0][0])
    # print(lt.fi_def(lt.df_vertex, 'q'))
    


if __name__ == '__main__':
    main()
    