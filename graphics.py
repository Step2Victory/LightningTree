import matplotlib.pyplot as plt
import copy


class Vector(object):
    def __init__(self, _point : list[float]):
        self.x = _point[0]
        self.y = _point[1]
        self.z = _point[2]
        
    def __str__(self):
        return "({}, {}, {})".format(self.x, self.y, self.z)


class Charge(object):
    def __init__(self, _point : Vector, _q : float, _Q : float):
        self.point = _point
        self.q = _q
        self.Q = _Q

    def __str__(self):
        return "{}, {}, {}".format(self.point, self.q, self.Q)
   
 
class Edge(object):
    def __init__ (self, _charges : list[Charge], _sigma : float):
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
                #print(_point, _q, _Q)
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
    #print(edges[1]._from)
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
        ax.plot([edge._from.point.x, edge._to.point.x],
                [edge._from.point.y, edge._to.point.y],
                [edge._from.point.z, edge._to.point.z], color="blue", alpha=0.4)
    
    ax.scatter(x, y, z, c=Q, cmap="plasma", alpha=0.2)
    plt.show()
    
def map2d_tree(edges, plane:str = "xz", var:float = 0):
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
                if(edge._from.point.y == var and edge._to.point.y == var):
                    ax.plot([edge._from.point.x, edge._to.point.x], 
                            [edge._from.point.z, edge._to.point.z], 
                            color="blue")
            case "yz" | "zy":
                if(edge._from.point.y == var and edge._to.point.y == var):
                    ax.plot([edge._from.point.y, edge._to.point.y], 
                            [edge._from.point.z, edge._to.point.z],
                            color="blue")
            case "xy" | "yx":
                if(edge._from.point.y == var and edge._to.point.y == var):
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
    
    
def main():
    edges = openTree("LightningTree.txt")
    map3d_tree(edges)
    #map2d_tree(edges)


if __name__ == '__main__':
    main()
    