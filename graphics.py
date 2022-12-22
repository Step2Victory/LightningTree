import matplotlib.pyplot as plt
import plotly.graph_objects as go
import pandas as pd

from jupyter_dash import JupyterDash
from dash import dcc, html


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


def map3d_tree(edges):
    # create 3d axes
    ax = plt.axes(projection='3d')
    ax.set_title("3D graph lighting tree")
    x = []
    y = []
    z = []
    q = []
    Q = []
    # print(edges[1]._from)
    for edge in edges:
        # x, y, z = edge._from.point // нужен iter и next
        # x, y, z = edge._to.point
        x.append(edge._from.point.x)
        x.append(edge._to.point.x)
        y.append(edge._from.point.y)
        y.append(edge._to.point.y)
        z.append(edge._from.point.z)
        z.append(edge._to.point.z)
        q.append(edge._from.q)
        q.append(edge._to.q)
        Q.append(edge._from.Q)
        Q.append(edge._to.Q)
        ax.plot([edge._from.point.x, edge._to.point.x],
                [edge._from.point.y, edge._to.point.y],
                [edge._from.point.z, edge._to.point.z], color="blue", alpha=0.4)

    ax.scatter(x, y, z, c=Q, cmap="plasma", alpha=0.2)
    plt.show()


def map2d_tree(edges, plane: str = "xz", var: float = 0):
    ax = plt.axes()
    ax.set_title("2D graph lighting tree in plane " + plane)
    x = []
    y = []
    z = []
    q = []
    Q = []
    for edge in edges:
        x.append(edge._from.point.x)
        x.append(edge._to.point.x)
        y.append(edge._from.point.y)
        y.append(edge._to.point.y)
        z.append(edge._from.point.z)
        z.append(edge._to.point.z)
        q.append(edge._from.q)
        q.append(edge._to.q)
        Q.append(edge._from.Q)
        Q.append(edge._to.Q)

        match plane:
            case "xz" | "zx":
                if (edge._from.point.y == var and edge._to.point.y == var):
                    ax.plot([edge._from.point.x, edge._to.point.x],
                            [edge._from.point.z, edge._to.point.z],
                            color="blue")
            case "yz" | "zy":
                if (edge._from.point.y == var and edge._to.point.y == var):
                    ax.plot([edge._from.point.y, edge._to.point.y],
                            [edge._from.point.z, edge._to.point.z],
                            color="blue")
            case "xy" | "yx":
                if (edge._from.point.y == var and edge._to.point.y == var):
                    ax.plot([edge._from.point.x, edge._to.point.x],
                            [edge._from.point.y, edge._to.point.y],
                            color="blue")

    match plane:
        case "xz" | "zx":
            ax.scatter(x, z, c=Q, cmap="plasma")
        case "yz" | "zy":
            ax.scatter(y, z, c=Q, cmap="plasma")
        case "xy" | "yx":
            ax.scatter(x, y, c=Q, cmap="plasma")
    plt.show()


def map3d_tree_plotly(edges, plane: str = "", value: int = 0, Range: list=[-10, 10], _mode:str='external'):
    app = JupyterDash('SimpleExemple')
    place = plane.upper()
    edge_x, edge_y, edge_z = [], [], []
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

        edge_x.append(None)
        edge_y.append(None)
        edge_z.append(None)

    df_edges = pd.DataFrame({"x": edge_x, "y": edge_y, "z": edge_z})
    df_nodes = pd.DataFrame({"x": node_x, "y": node_y, "z": node_z, "q": node_q, "Q": node_Q})

    setting = {'showbackground': False, 'showticklabels': False, 'showgrid': False, 'zeroline': False}
    layout = go.Layout(showlegend=False, hovermode='closest',
                       scene={'xaxis': setting, 'yaxis': setting, 'zaxis': setting})

    if (value != 0 or Range != [-10, 10]) and place == "":
        print("Не введена плоскость!")

    if place != "":
        if (value != 0) and (Range == [-10, 10]):
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

    edge_trace = go.Scatter3d(x=df_edges.x, y=df_edges.y, z=df_edges.z,
                              line=dict(width=2, color='lightblue'), hoverinfo='none', mode='lines')

    node_trace = go.Scatter3d(x=df_nodes.x, y=df_nodes.y, z=df_nodes.z,
                              mode='markers',
                              hoverinfo='text',
                              marker=dict(showscale=True,
                                          colorscale='viridis',
                                          color=df_nodes.Q,
                                          cmin=-1,
                                          cmax=1,
                                          size=2                                       
                                          ),
                              line_width=.1
                              )
    node_trace.text = df_nodes.Q

    data = [edge_trace, node_trace]
    fig = go.Figure(data=data, layout=layout)
    app.layout = html.Div([
        html.H1("Граф дерева молнии", style={'textAlign': 'center', 'color': 'gold'}),
        dcc.Graph(figure=fig, style={'height': '120vh'})
    ])

    app.run_server(mode=_mode)  # inline - внутри jupyter; external - в браузере


def main():
    edges = openTree("LightningTree.txt")
    # map3d_tree(edges)
    # map2d_tree(edges)
    map3d_tree_plotly(edges)#, "xz", Range=[-100, 0])


if __name__ == '__main__':
    main()
    