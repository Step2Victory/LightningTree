#include <vector>
#include <cstdlib>
#include <cmath>
#include "ExternalField.h"



class LightningTree
{
    struct Charge
    {
        Vector point;
        double q;
        double Q;
    };

    struct Edge
    {
        Charge* from;
        Charge* to;
        double sigma;
    };

    std::map<Charge*, std::array<Edge*, 26>> graph;
    std::vector<Edge*> edges;
    std::vector<Charge*> charges;

    uint64_t iter_number;

    double h;
    double a_x;
    double a_y;
    double a_z;
    double b_x;
    double b_y;
    double b_z;
    double t;
    double T;
    double r = 0.01;
    double R = 0.1;
    double q_max = 1;
    double resistance = 1;
    double E_plus;
    double eta = 0;
    double beta = 0;
    double sigma;
    ExternalField phi_a;
public:

    LightningTree();
    LightningTree(double h,
    double a_x, double a_y, double a_z, double b_x, double b_y, double b_z,
    double t, double T, double r, double R, double q_max, double resistance,
    double E_plus, double eta, double beta, double sigma, ExternalField phi_a)

    : h(h), a_x(a_x), a_y(a_y), a_z(a_z), b_x(b_x), b_y(b_y), b_z(b_z), t(t), T(T),
    r(r), R(R), q_max(q_max), resistance(resistance), E_plus(E_plus), eta(eta), beta(beta),
    sigma(sigma), phi_a(phi_a), iter_number(0)
    {}


    void NextIter() // combine charges and edges count
    {
        
        
    }

    void NextIterCharges() // count new charges
    {}
    
    void NextIterEdges() // count new edges
    {}    

    std::map<Charge*, std::array<Edge*, 26>> GetGraph() const;

    

};