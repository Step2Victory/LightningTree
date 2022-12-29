import matplotlib.pyplot as plt
import plotly.graph_objects as go
import pandas as pd

from jupyter_dash import JupyterDash
from dash import dcc, html
from dash.dependencies import Output, Input


class Vector(object):
    def __init__(self, _point: list[float]):
        self.x = _point[0]
        self.y = _point[1]
        self.z = _point[2]

    def __str__(self):
        return "({}, {}, {})".format(self.x, self.y, self.z)

    # def __iter__(self):
    #     return self

    # def __next__(self):
    #     return self


class Charge(object):
    def __init__(self, _point: Vector, _q: float, _Q: float):
        self.point = _point
        self.q = _q
        self.Q = _Q

    def __str__(self):
        return "{}, {}, {}".format(self.point, self.q, self.Q)


class Edge(object):
    def __init__(self, _charges: list[Charge], _sigma: float):
        self._from = _charges[0]
        self._to = _charges[1]
        self.sigma = _sigma

    def __str__(self):
        return "{}; {}; {}".format(self._from, self._to, self.sigma)


def openTree(filename):
    edges = []
    with open(filename, 'r') as file:
        text = file.read()
        charges = []
        for value in text.splitlines():
            _sigma = float(value.rsplit(';', 1)[1])
            for charge in (value.rsplit(';', 1)[0]).split(';'):
                _q = float(charge.strip().rsplit(',', 2)[1])
                _Q = float(charge.strip().rsplit(',', 2)[2])
                _point = [float(point.strip('( )')) for point in (charge.strip().rsplit(',', 2)[0]).split(',')]
                # print(_point, _q, _Q)
                charges.append(Charge(Vector(_point.copy()), _q, _Q))
            edges.append(Edge(charges, _sigma))
            charges.clear()
    return edges

def parseTreeToDF(edges):
    edge_x, edge_y, edge_z = [], [], []
    edge_color = []
    node_x, node_y, node_z = [], [], []
    node_q, node_Q = [], []
    for edge in edges:
        edge_x.append(edge._from.point.x)
        edge_y.append(edge._from.point.y)
        edge_z.append(edge._from.point.z)

        node_x.append(edge._from.point.x)
        node_y.append(edge._from.point.y)
        node_z.append(edge._from.point.z)

        node_q.append(edge._from.q)
        node_Q.append(edge._from.Q)

        edge_x.append(edge._to.point.x)
        edge_y.append(edge._to.point.y)
        edge_z.append(edge._to.point.z)

        node_x.append(edge._to.point.x)
        node_y.append(edge._to.point.y)
        node_z.append(edge._to.point.z)

        node_q.append(edge._to.q)
        node_Q.append(edge._to.Q)
        
        if(edge._to.point.z - edge._from.point.z > 0):
            edge_color.append(1)
            edge_color.append(1)
        else:
            edge_color.append(-1)
            edge_color.append(-1)

        edge_x.append(None)
        edge_y.append(None)
        edge_z.append(None)
        edge_color.append(None)

    df_edges = pd.DataFrame({"x": edge_x, "y": edge_y, "z": edge_z, 'color': edge_color})
    df_nodes = pd.DataFrame({"x": node_x, "y": node_y, "z": node_z, "q": node_q, "Q": node_Q})
    
    return df_edges, df_nodes

def slice_selection(df_edges, df_nodes, place, value, Range):       
    if place != "":
        if (Range == [-10, 10]) and (value != 0):
            Range = [Range[0] + value, Range[1] + value]

        low, high = Range
        mask_nodes = True
        mask_edges = True
        match place:
            case "XZ" | "ZX":
                mask_nodes = (df_nodes.y >= low) & (df_nodes.y <= high)
                mask = []
                for i in range(0, len(df_edges.y), 3):
                    if (low <= df_edges.y[i] <= high) and (low <= df_edges.y[i + 1] <= high):
                        mask.append(True)
                        mask.append(True)
                        mask.append(True)
                    else:
                        mask.append(False)
                        mask.append(False)
                        mask.append(False)
                mask_edges = pd.Series(data=mask)
            case "YZ" | "ZY":
                mask_nodes = (df_nodes.x >= low) & (df_nodes.x <= high)
                mask = []
                for i in range(0, len(df_edges.x), 3):
                    if (low <= df_edges.x[i] <= high) and (low <= df_edges.x[i + 1] <= high):
                        mask.append(True)
                        mask.append(True)
                        mask.append(True)
                    else:
                        mask.append(False)
                        mask.append(False)
                        mask.append(False)
                mask_edges = pd.Series(data=mask)
            case "XY" | "YX":
                mask_nodes = (df_nodes.z >= low) & (df_nodes.z <= high)
                mask = []
                for i in range(0, len(df_edges.z), 3):
                    if (low <= df_edges.z[i] <= high) and (low <= df_edges.z[i + 1] <= high):
                        mask.append(True)
                        mask.append(True)
                        mask.append(True)
                    else:
                        mask.append(False)
                        mask.append(False)
                        mask.append(False)
                mask_edges = pd.Series(data=mask)
        df_edges = df_edges[mask_edges]
        df_nodes = df_nodes[mask_nodes]   
    elif value != 0 or Range != [-10, 10]:
        print("Не введена плоскость!")
        
    return df_edges, df_nodes

def map3d_tree_plotly(filename, place: str = "", value: int = 0, Range: list=[-10, 10], mode:str='external', interval:float=5):
    app = JupyterDash('SimpleExemple')
    place = place.upper()
    scale_nodes = [(0, "darkblue"), (0.15, "blue"), (0.49, "yellow"), (0.5, "gray"), (0.51, "yellow"), (0.85, "red"), (1, "darkred")] # цветовая шкала для зарядов
    scale_case = [(0, "darkblue"), (0.15, "blue"), (0.49, "yellow"), (0.5, "white"), (0.51, "yellow"), (0.85, "red"), (1, "darkred")] # цветовая шкала для чехлов
    setting = {'showbackground': False, 'showticklabels': False, 'showgrid': False, 'zeroline': False}
    layout = go.Layout(showlegend=False, hovermode='closest',
                       scene={'xaxis': setting, 'yaxis': setting, 'zaxis': setting}
                       )
    
    app.layout = html.Div([html.H1("Граф дерева молнии", 
                                   style={'textAlign': 'center', 'color': 'gold'}),
                           dcc.Graph(id='live-update-graph',
                                     style={'height': '100vh'}),
                           dcc.Interval(id='interval-component',
                                        interval=interval*1000,
                                        n_intervals=0)
                           ])
    
    @app.callback(Output('live-update-graph', 'figure'),
                  Input('interval-component', 'n_intervals'))
    def update_graph_live(n):
        edges = openTree(filename)
        # Разбор файла
        df_edges, df_nodes = parseTreeToDF(edges)
        # Выделение среза среза, если задана плоскость
        df_edges, df_nodes = slice_selection(df_edges, df_nodes, place, value, Range)
                        
        # Построение рёбер
        edge_trace = go.Scatter3d(x=df_edges.x, y=df_edges.y, z=df_edges.z,
                                line=dict(width=2, color=df_edges.color, colorscale=["darkslateblue", "crimson"], cmin=-1, cmax=1), 
                                hoverinfo='none', 
                                mode='lines'
                                )
        
        # Построение зарядов    
        node_trace = go.Scatter3d(x=df_nodes.x, y=df_nodes.y, z=df_nodes.z,
                                mode='markers',
                                marker=dict(showscale=True,
                                            colorscale=scale_nodes,
                                            color=df_nodes.Q,
                                            cmin=-0.01,
                                            cmax=0.01,
                                            size=2.4,
                                            ),
                                line_width=.1
                                )
        
        # Построение чехлов
        case_trace = go.Scatter3d(x=df_nodes.x, y=df_nodes.y, z=df_nodes.z,
                                mode='markers',
                                text = df_nodes.q,
                                customdata= df_nodes.Q,
                                hovertemplate='q= %{customdata} <br>Q= %{text}<extra></extra>',
                                marker=dict(showscale=False,
                                            colorscale=scale_case,
                                            color=df_nodes.q,
                                            cmin=-0.0001,
                                            cmax=0.0001,
                                            size=24
                                            ),
                                line_width=1,
                                opacity=0.1
                                )
        
        data = [edge_trace, node_trace, case_trace]
        fig = go.Figure(data=data, layout=layout)   
             
        return fig

    app.run_server(mode=mode)  # inline - внутри jupyter; external - в браузере

def main():
    # edges = openTree("LightningTree.txt")
    map3d_tree_plotly("LightningTree.txt")#, "xz", Range=[-100, 0])


if __name__ == '__main__':
    main()
    