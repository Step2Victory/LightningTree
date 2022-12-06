#pragma once
#include <vector>
#include <cstdlib>
#include <cmath>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <utility>
#include "ExternalField.h"


struct Charge
{
    Vector point;
    double q;
    double Q;
    Charge(const Vector& point, double q, double Q) : point(point), q(q), Q(Q) {};
    std::shared_ptr<Charge> GetMirror();

    // auto operator<=>(const Charge& rhs) const = default;

    std::string tostring() const;
};

using ChargePtr = std::shared_ptr<Charge>;

struct Edge
{
    ChargePtr from;
    ChargePtr to;
    double sigma;
    Edge(ChargePtr from, ChargePtr to, double sigma) : from(from), to(to), sigma(sigma) {};
};

using EdgePtr = std::shared_ptr<Edge>;


class LTBuilder;
class LightningTree
{
    std::unordered_map<ChargePtr, std::vector<EdgePtr>> graph;
    std::unordered_set<EdgePtr> edges;
    std::unordered_set<ChargePtr> charges;
    
    uint64_t iter_number_charges;
    uint64_t iter_number_edges;

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
    double E_plus = 150'000;
    double E_minus;
    double eta = 0;
    double beta = 0;
    double sigma;
    ExternalField phi_a;
    

    double qCountPotential(ChargePtr charge, const Vector& point);

    double qCountPotential(const Vector& point); // реализация формулы (4) из Leaders.pdf

    double QCountPotential(ChargePtr charge, const Vector& point);

    double QCountPotential( const Vector& point);

    double ElectricFieldAlongEdge(EdgePtr edge);

    double CurrentAlongEdge(EdgePtr edge); // реализация формулы (8) из Leaders.pdf

    double ElectricFieldAlongEdgeRad(EdgePtr edge);

    double Heaviside(double x);

    double CurrentSheath(ChargePtr charge); // реализация формулы (10) из Leaders.pdf

    bool MakeEdge(EdgePtr edge); // содержит реализацию формулы (12) из Leaders.pdf

    bool Find(ChargePtr charge, const std::vector<EdgePtr>& edges); // проверяет является заряд charge концом какого-нибудь ребра из edges

    ChargePtr GetChargeInPoint(Vector point); 

    LightningTree(double h, double x_min, double y_min, double z_min, double x_max, double y_max, 
    double z_max, double delta_t, double delta_T, double r, double R, double q_plus_max, double q_minus_max, double resistance,
    double E_plus, double E_minus, double eta, double beta, double sigma, ExternalField phi_a, 
    std::unordered_map<ChargePtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<ChargePtr> charges); 
    friend LTBuilder;
public:
    void NextIter(); // combine charges and edges count

    void NextIterCharges(); // count new charges

    
    void NextIterEdges(); // count new edges using dis 

    std::unordered_map<ChargePtr, std::vector<EdgePtr>> GetGraph() const; // возвращает копию текущего состояния графа

    void Info();

    ~LightningTree(){};
};