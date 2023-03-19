import plotly.graph_objects as go
import pandas as pd

from jupyter_dash import JupyterDash
from dash import dcc, html
# from dash.dependencies import Output, Input


class LightningTree(object):
    def __init__ (self, folder):
        self.df_vertex = self.open_file(folder+'/vertex_table.txt')
        self.df_edge = self.open_file(folder+'/edge_table.txt')
        # self.df_q_history = self.open_file(folder+'/q_history_1.txt')
        # self.df_Q_history = self.open_file(folder+'/Q_history.txt')

    def open_file(self, filename: str):
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
    
    # Метод расчета значений по группе
    def distribution(self, df:pd.DataFrame, groupby:str, operarion:str, colomn:str):
        df_result = df.groupby([groupby], ).agg({colomn:[operarion]})
        return df_result

    # Создание 2D графика
    def plot(self, operation:str):
        df_q = self.distribution(self.df_vertex, 'z', operation, 'q')
        df_Q = self.distribution(self.df_vertex, 'z', operation, 'Q')
        data = [go.Scatter(x=df_q.values[::-1].ravel(), y=df_q.index.values[::-1], 
                           mode='lines+markers', name=operation + ' q'),
                go.Scatter(x=df_Q.values[::-1].ravel(), y=df_Q.index.values[::-1], 
                           mode='lines+markers', name=operation + ' Q')]
        fig = go.Figure(data=data, layout={'uirevision': 'True'})
        return fig

    # Создание 3D графа дерева молнии
    def plot_tree(self):
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
                                  line=dict(width=2, colorscale=["darkslateblue", "crimson"], cmin=-1, cmax=1),
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
                                  marker=dict(showscale=False, colorscale=scale_case, color=self.df_vertex.Q, cmin=-0.000001, cmax=0.000001, size=24),
                                  text = self.df_vertex.q,
                                  customdata= self.df_vertex.Q,
                                  hovertemplate='q= %{text} <br>Q= %{customdata}<extra></extra>',
                                  line_width=1,
                                  opacity=0.1)
        
        data = [edge_trace, node_trace, case_trace]
        result = go.Figure(data=data, layout=layout)
        return result
    
    # Метод для запуска Dash-приложения
    def run(self):
        # Создание Dash-приложения
        app = JupyterDash('SimpleExemple')

        # Настройка и запуск Dash-приложения
        app.layout = html.Div([html.Div([html.H4("Сумма распределения зарядов"),
                                        dcc.Graph(figure=(self.plot('mean')),
                                                style={'height': '90vh'})
                                        ], style={'display': 'inline-block', 'width': '30%'}),
                            html.Div([html.H1("Граф дерева молнии",
                                                style={'textAlign': 'center', 'color': 'gold'}),
                                        dcc.Graph(figure=(self.plot_tree()),
                                                style={'height': '90vh'})
                                        ], style={'display': 'inline-block', 'width': '70%'})])
        app.run_server(mode='external')  # inline - внутри jupyter; external - в браузере


def main():
    lt = LightningTree("LightningTree_data")
    lt.run()
    # print(lt.distribution(lt.df_vertex, 'z', 'sum', 'q').values)


if __name__ == '__main__':
    main()
    