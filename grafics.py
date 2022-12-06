import matplotlib.pyplot as plt


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
                charges.append(Charge(Vector(_point.copy()), _q, _Q))
            edges.append(Edge(charges.copy(), _sigma))       
    return edges
            
def map3d_tree(edges):
    # create 3d axes
    ax = plt.axes(projection='3d')
    ax.set_title("3D plots lighting tree")
    x = []
    y = []
    z = []
    q = []
    for edge in edges:
        x.append(edge._from.point.x)
        x.append(edge._to.point.x)
        y.append(edge._from.point.y)
        y.append(edge._to.point.y)
        z.append(edge._from.point.z)
        z.append(edge._to.point.z)
        q.append(edge._from.q)
        q.append(edge._to.q)
        ax.plot([edge._from.point.x, edge._to.point.x],
                [edge._from.point.y, edge._to.point.y],
                [edge._from.point.z, edge._to.point.z])
    
    ax.scatter(x, y, z, c=2, cmap='viridis', label=q)
    plt.show()
    
def main():
    edges = openTree("LightningTree.txt")
    map3d_tree(edges)


if __name__ == '__main__':
    main()
    