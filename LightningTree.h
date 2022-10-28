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
    double x_min;
    double y_min;
    double z_min;
    double x_max;
    double y_max;
    double z_max;
    double delta_t;
    double delta_T;
    double r = 0.01;
    double R = 0.1;
    double q_plus_max = 1;
    double q_minus_max = -1;
    double resistance = 1;
    double E_plus;
    double eta = 0;
    double beta = 0;
    double sigma;
    ExternalField phi_a;
public:

    LightningTree();
    LightningTree(double h,
    double x_min, double y_min, double z_min, double x_max, double y_max, double z_max,
    double delta_t, double delta_T, double r, double R, double q_plus_max, double q_minus_max, double resistance,
    double E_plus, double eta, double beta, double sigma, ExternalField phi_a)

    : h(h), x_min(x_min), y_min(y_min), z_min(z_min), x_max(x_max), y_max(y_max), z_max(z_max), delta_t(delta_t), delta_T(delta_T),
    r(r), R(R), q_plus_max(q_plus_max), q_minus_max(q_minus_max), resistance(resistance), E_plus(E_plus), eta(eta), beta(beta),
    sigma(sigma), phi_a(phi_a), iter_number(0)
    {}


    void NextIter() // combine charges and edges count
    {}

    void NextIterCharges() // count new charges
    {}
    
    void NextIterEdges() // count new edges
    {}    

    std::map<Charge*, std::array<Edge*, 26>> GetGraph() const;

    

};