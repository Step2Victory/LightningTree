#include <vector>
#include <cstdlib>
#include <cmath>
#include <random>
#include "ExternalField.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1.0);


constexpr double epsilon_0 = 8.85418781762 * 1e-12;
constexpr double pi = 3.1415926535;

constexpr double kEps = 1e-9;

class LightningTree
{
    struct Charge
    {
        Vector point;
        double q;
        double Q;
        Charge GetMirror()
        {
            return {.point={point.data[0], point.data[1], -point.data[2]}, .q=-q, .Q=-Q};
        }
    };

    struct Edge
    {
        Charge* from;
        Charge* to;
        double sigma;
    };

    std::map<Charge*, std::vector<Edge*>> graph;
    std::vector<Edge*> edges;
    std::vector<Charge*> charges;
    

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
    

    double qCountPotential(const Charge& charge, const Vector& point)
    {
        return charge.q / (4 * pi * epsilon_0 * (Abs(charge.point - point) + r));
    }

    double qCountPotential(const std::vector<Charge*>& charge, const Vector& point) // реализация формулы (4) из Leaders.pdf
    {
        double ans = 0;
        for (size_t i = 0; i < charge.size(); ++i)
        {
            ans += qCountPotential(*charge[i], point) - qCountPotential(charge[i]->GetMirror(), point);
        }
        return ans;
    }

    double QCountPotential(const Charge& charge, const Vector& point)
    {
        return charge.Q / (4 * pi * epsilon_0 * (Abs(charge.point - point) + R));
    }

    double QCountPotential(const std::vector<Charge*>& charge, const Vector& point) // реализация формулы (5) из Leaders.pdf
    {
        double ans = 0;
        for (size_t i = 0; i < charge.size(); ++i)
        {
            ans += QCountPotential(*charge[i], point) - QCountPotential(charge[i]->GetMirror(), point);
        }
        return ans;
    }

    double ElectricFieldAlongEdge(const Edge* edge) // реализация формулы (7) из Leaders.pdf
    {
        double l = Abs(edge->from->point - edge->to->point);
        double phi_1 = qCountPotential(charges, edge->from->point) + QCountPotential(charges, edge->from->point) + phi_a.getValue(edge->from->point); // формула (6)
        double phi_2 = qCountPotential(charges, edge->to->point) + QCountPotential(charges, edge->to->point) + phi_a.getValue(edge->to->point);
        return -(phi_1 - phi_2) / l;
    }

    double CurrentAlongEdge(const Edge* edge) // реализация формулы (8) из Leaders.pdf
    {
        return pi * r * r * edge->sigma * ElectricFieldAlongEdge(edge);
    }

    double ElectricFieldAlongEdgeRad(const Edge* edge)
    {
        return 0;
    }

    double Heaviside(double x)
    {
        if (x > 0)
        {
            return 1;
        }
        return 0;
    }

    double CurrentSheath(const Charge& charge) // реализация формулы (10) из Leaders.pdf
    {
        if (charge.q > 0)
        {
            return resistance * charge.q / abs(charge.q) * Heaviside((abs(charge.q) - q_plus_max) / r);
        }
        return resistance * charge.q / abs(charge.q) * Heaviside((abs(charge.q) - q_minus_max) / r);  
    }

    bool MakeEdge(Edge* edge) 
    {
        double probability = dis(gen);
        double E = ElectricFieldAlongEdge(edge);
        // реализация формулы (12) из Leaders.pdf
        if (E > 0)
        {
            return (1 - std::exp(-std::pow((E / E_plus), 2.5))) > probability;
        }
        return (1 - std::exp(-std::pow((E / E_minus), 2.5))) > probability;
    }

    bool Find(Charge * charge, std::vector<Edge*> edges) // проверяет является заряд charge концом какого-нибудь ребра из edges
    {
        for (auto * edge : edges)
        {
            if (Abs(edge->to->point - charge->point) < kEps)
            {
                return true;
            }
        }
        return false;
    }

public:

    LightningTree();
    LightningTree(double h,
    double x_min, double y_min, double z_min, double x_max, double y_max, double z_max,
    double delta_t, double delta_T, double r, double R, double q_plus_max, double q_minus_max, double resistance,
    double E_plus, double eta, double beta, double sigma, ExternalField phi_a)

    : h(h), x_min(x_min), y_min(y_min), z_min(z_min), x_max(x_max), y_max(y_max), z_max(z_max), delta_t(delta_t), delta_T(delta_T),
    r(r), R(R), q_plus_max(q_plus_max), q_minus_max(q_minus_max), resistance(resistance), E_plus(E_plus), eta(eta), beta(beta),
    sigma(sigma), phi_a(phi_a), iter_number_charges(0), iter_number_edges(0)
    {
        E_minus = 2 * E_plus;
    }


    void NextIter() // combine charges and edges count
    {
        NextIterCharges();
        if (delta_t * iter_number_charges > delta_T * iter_number_edges)
        {   
            NextIterEdges();
        }
    }

    void NextIterCharges() // count new charges
    {
        std::map<Charge*, std::pair<double, double>> delta_charges;
        for (auto elem : graph)
        {
            Charge* charge = elem.first;
            std::vector<Edge*> edges = elem.second;
            for (auto* edge: edges)
            {
                delta_charges[charge].first += CurrentAlongEdge(edge);
            }
            delta_charges[charge].first-= CurrentSheath(*charge);
            delta_charges[charge].second = CurrentSheath(*charge);
        }
        for (auto elem : delta_charges)
        {
            elem.first->q += delta_charges[elem.first].first * delta_t;
            elem.first->Q += delta_charges[elem.first].second * delta_t;
        }
        iter_number_charges++;
    }
    
    void NextIterEdges() // count new edges using dis
    {

        for (auto elem : graph)
        {
            Charge* charge = elem.first;
            std::vector<Edge*> edges = elem.second;
            for (int i = -1; i < 2; ++i)
            {
                for (int j = -1; j < 2; ++j)
                {
                    for (int k = -1; k < 2; ++k)
                    {
                        Charge * new_charge = new Charge{.point = charge->point + Vector{i * h, j * h, k * h}, .q = 0, .Q = 0};
                        Edge * edge = new Edge{.from = charge, .to = new_charge, .sigma = sigma};
                        if (!Find(new_charge, edges) && MakeEdge(edge))
                        {
                            edges.push_back(edge);
                            charges.push_back(new_charge);
                            graph[charge].push_back(edge);
                            // graph[new_charge].push_back(new Edge{.from = new_charge, .to = charge, .sigma = sigma});
                        }
                        else
                        {
                            delete edge;
                            delete new_charge;
                        }
                    }
                }
            }
        }
        iter_number_edges++;
    }    

    std::map<Charge, std::vector<Edge>> GetGraph() const // возвращает копию текущего состояния графа
    {
        std::map<Charge, std::vector<Edge>> ans;
        for (auto elem: graph)
        {
            for (auto edge: elem.second)
            {
                ans[*elem.first].push_back(*edge);
            }
        }
        return ans;
    }

    ~LightningTree() // деструктор освобождает память
    {
        for (auto edge: edges)
        {
            delete edge;
        }
        for (auto charge: charges)
        {
            delete charge;
        }   
    }

};