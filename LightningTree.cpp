#include "LightningTree.h"


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.5, 1);

constexpr double epsilon_0 = 8.85418781762 * 1e-12;
constexpr double pi = 3.1415926535;

constexpr double kEps = 1e-9;

// Charge
////////////////
ChargePtr Charge::GetMirror()
{
    return std::make_shared<Charge>(Vector{point.data[0], point.data[1], -point.data[2]},-q, -Q);
}


// Lightning Tree
/////////////////

//Potential Count Functions
/////////////////
double LightningTree::qCountPotential(ChargePtr charge, const Vector& point)
{
    if (Abs(charge->point - point) < kEps)
    {
        return 0;
    }
    return charge->q / (4 * pi * epsilon_0 * (Abs(charge->point - point) + r));
}

double LightningTree::qCountPotential(const Vector& point) // реализация формулы (4) из Leaders.pdf
{
    double ans = 0;
    for (auto charge: charges)
    {
        ans += qCountPotential(charge, point) - qCountPotential(charge->GetMirror(), point);
    }
    return ans;
}

double LightningTree::QCountPotential(ChargePtr charge, const Vector& point)
{
    if (Abs(charge->point - point) < kEps)
    {
        return 0;
    }
    return charge->Q / (4 * pi * epsilon_0 * (Abs(charge->point - point) + R));
}

double LightningTree::QCountPotential(const Vector& point) // реализация формулы (5) из Leaders.pdf
{
    double ans = 0;
    for (auto charge: charges)
    {
        ans += QCountPotential(charge, point) - QCountPotential(charge->GetMirror(), point);
    }
    return ans;
}

// Electric Field and Current count fuctions
///////////////////////////////////////////
double LightningTree::ElectricFieldAlongEdge(EdgePtr edge) // реализация формулы (7) из Leaders.pdf
{
    double l = Abs(edge->from->point - edge->to->point);
    double phi_1 = qCountPotential(edge->from->point) + QCountPotential(edge->from->point) + phi_a.getValue(edge->from->point); // формула (6)
    double phi_2 = qCountPotential(edge->to->point) + QCountPotential(edge->to->point) + phi_a.getValue(edge->to->point);
    return -(phi_1 - phi_2) / l;
}

double LightningTree::CurrentAlongEdge(EdgePtr edge) // реализация формулы (8) из Leaders.pdf
{
    return pi * r * r * edge->sigma * ElectricFieldAlongEdge(edge);
}

double LightningTree::ElectricFieldAlongEdgeRad(EdgePtr edge)
{
    return 0;
}

double LightningTree::Heaviside(double x)
{
    if (x > 0)
    {
        return 1;
    }
    return 0;
}

double LightningTree::CurrentSheath(ChargePtr charge) // реализация формулы (10) из Leaders.pdf
{
    if (std::abs(charge->q) <= kEps)
    {
        return 0;
    }
    if (charge->q > kEps)
    {
        return resistance * charge->q / std::abs(charge->q) * Heaviside((std::abs(charge->q) - q_plus_max) / r);
    }
    if (charge->q < kEps)
    {
        return resistance * charge->q / std::abs(charge->q) * Heaviside((std::abs(charge->q) - q_minus_max) / r);
    }
    return -1;
}
// Helper functions for making new edge
//////////////////////////////////
bool LightningTree::MakeEdge(EdgePtr edge) 
{
    double probability = dis(gen);
    double E = ElectricFieldAlongEdge(edge);
    
    // реализация формулы (12) из Leaders.pdf
    if (E > E_plus)
    {
        // std::cout << E << ' ' << (1 - std::exp(-std::pow((E / E_plus), 2.5))) << std::endl;
        return (1 - std::exp(-std::pow(((E - E_plus)/ E_plus), 1))) > probability;
    }
    else if (-E > E_minus)
    {
        // std::cout << E << ' ' << (1 - std::exp(-std::pow((E / E_plus), 2.5))) << std::endl;
        return (1 - std::exp(-std::pow(((-E - E_minus)/ E_minus), 1))) > probability;
    }
    
    return false;

    // std::cout << E << ' ' << (1 - std::exp(-std::pow((-E / E_minus), 2.5))) << std::endl;
    // if (E > 0)
    // {
    //     // std::cout << E << ' ' << (1 - std::exp(-std::pow((E / E_plus), 2.5))) << std::endl;
    //     return (1 - std::exp(-std::pow(((E - E_plus)/ E_plus), 2.5))) > probability;
    // }
    // // std::cout << E << ' ' << (1 - std::exp(-std::pow((-E / E_minus), 2.5))) << std::endl;
    // return (1 - std::exp(-std::pow(((-E - E_minus)/ E_minus), 2.5))) > probability;
}

void LightningTree::DeleteCharge(ChargePtr charge) // удаляет заряд и все его ребра из дерева
{
    if (graph.contains(charge))
    {
        for (auto edge: graph[charge])
        {
            if (edge->from == charge)
            {
                auto iter_in_to = std::find(graph[edge->to].begin(), graph[edge->to].end(), edge);
                if (iter_in_to != graph[edge->to].end())
                    graph[edge->to].erase(iter_in_to);
            
                
                if (graph[edge->to].size() == 1)
                {
                    peripheral[0].push_back(edge->to);
                }
            }
            else
            {
                auto iter_in_from = std::find(graph[edge->from].begin(), graph[edge->from].end(), edge);
                graph[edge->from].erase(iter_in_from);
                if (graph[edge->from].size() == 1)
                {
                    peripheral[0].push_back(edge->from);
                }
            }
            edges.erase(edge);
        }
        graph.erase(charge);
    }
}

void LightningTree::DeleteFromPerephery(ChargePtr charge)
{
    for (size_t i = 0; i < kNumberOfIters; ++i)
    {
        if (auto charge_iter = std::find(peripheral[i].begin(), peripheral[i].end(), charge); charge_iter != peripheral[i].end())
        {
            peripheral[i].erase(charge_iter);
            return;
        }
    }
}

std::optional<ChargePtr> LightningTree::FindChargeInPoint(const Vector& point) // возвращает заряд в точке point, если такого нет, возвращает nullopt
{
    for (auto charge: charges)
    {
        if (Abs(charge->point - point) < kEps)
        {
            return charge;
        }
    }
    return std::nullopt;
}

bool LightningTree::FindInTree(const Vector& point)
{
    for (auto edge : edges)
    {
        if (Abs(edge->to->point - point) < kEps || Abs(edge->from->point - point) < kEps)
        {
            return true;
        }
    }
    return false;
}

bool LightningTree::FindInGivenEdges(const Vector& point, const std::vector<EdgePtr>& edges) // проверяет является заряд в точке point концом какого-нибудь ребра из edges
{
    for (auto edge : edges)
    {
        if (Abs(edge->to->point - point) < kEps || Abs(edge->from->point - point) < kEps)
        {
            return true;
        }
    }
    return false;
}

ChargePtr LightningTree::CreateChargeInPoint(const Vector& point) // создает заряж в точке point
{
    for (auto charge: charges)
    {
        if (Abs(charge->point - point) < kEps)
        {
            return nullptr;
        }
    }
    return std::make_shared<Charge>(point, 0, 0);
}

// Private Constructor for builder
////////////////////////////////////
LightningTree::LightningTree(double h, double x_min, double y_min, double z_min, double x_max, double y_max, 
double z_max, double delta_t, double delta_T, double r, double R, double q_plus_max, double q_minus_max, double resistance,
double E_plus, double E_minus, double eta, double beta, double sigma, ExternalField phi_a, 
std::unordered_map<ChargePtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<ChargePtr> charges)
: iter_number_charges(0), iter_number_edges(0),
h(h), x_min(x_min), y_min(y_min), z_min(z_min), x_max(x_max), y_max(y_max), z_max(z_max), delta_t(delta_t), delta_T(delta_T), r(r), R(R),
q_plus_max(q_plus_max), q_minus_max(q_minus_max), resistance(resistance), E_plus(E_plus), E_minus(E_minus), eta(eta), beta(beta), sigma(sigma),
phi_a(phi_a), graph(graph), edges(edges), charges(charges) {};

// Main Logic Functions
/////////////////////////////////////////////
void LightningTree::NextIter() // combine charges and edges count
{
    NextIterCharges();
    if (delta_t * iter_number_charges > delta_T * iter_number_edges)
    {   
        NextIterEdges();
        DeletePeripheral();
    }
}

void LightningTree::NextIterCharges() // count new charges
{
    std::unordered_map<ChargePtr, std::pair<double, double>> delta_charges;
    for (auto elem : graph)
    {
        ChargePtr charge = elem.first;
        std::vector<EdgePtr> edges = elem.second;
        for (auto edge: edges)
        {
            if (edge->from == charge)
            {
                delta_charges[charge].first += CurrentAlongEdge(edge);
            }
            else
            {
                delta_charges[charge].first -= CurrentAlongEdge(edge);
            }
        }
        delta_charges[charge].first-= CurrentSheath(charge);
        delta_charges[charge].second = CurrentSheath(charge);
    }
    for (auto elem : delta_charges)
    {
        elem.first->q += delta_charges[elem.first].first * delta_t;
        elem.first->Q += delta_charges[elem.first].second * delta_t;
    }
    iter_number_charges++;
}

void LightningTree::DeletePeripheral() // delete peripherical edges
{
    for (auto charge: peripheral[kNumberOfIters - 1])
    {
        DeleteCharge(charge);
    }
    for (int i = kNumberOfIters - 1; i > 0; --i)
    {
        std::swap(peripheral[i], peripheral[i - 1]);
    }
    peripheral[0] = std::vector<ChargePtr>();
}
    
    
void LightningTree::NextIterEdges() // count new edges using dis
{
    std::unordered_map<ChargePtr, std::vector<EdgePtr>> new_graph;
    for (auto elem : graph)
    {
        ChargePtr charge = elem.first;
        std::vector<EdgePtr> edges_in_current_elem = elem.second;
        for (int i = -1; i < 2; ++i)
        {
            for (int j = -1; j < 2; ++j)
            {
                for (int k = -1; k < 2; ++k)
                {
                    bool find_in_point = false;
                    Vector point = charge->point + Vector{i * h, j * h, k * h};
                    if (FindInTree(point))
                    {
                        continue;
                    }
                    ChargePtr new_charge = CreateChargeInPoint(point);
                    if (auto charge_in_point = FindChargeInPoint(point); charge_in_point.has_value())
                    {
                        find_in_point = true;
                        new_charge = *charge_in_point;
                    }
                    if (EdgePtr edge = std::make_shared<Edge>(charge, new_charge, sigma); MakeEdge(edge))
                    {
                        edges.insert(edge);
                        graph[charge].push_back(edge);
                        if (!find_in_point)
                        {
                            charges.insert(new_charge);
                        }
                        DeleteFromPerephery(charge);
                        new_graph[new_charge].push_back(edge);
                        peripheral[0].push_back(new_charge);
                    }
                }
            }
        }
    }
    graph.insert(new_graph.begin(), new_graph.end());
    iter_number_edges++;
}

/// нужны флаги свобоного заряда (если заряд имеет одно ребро)
// void LightningTree::DelEdges() // delete edges that don't create new ones 
// {
//     std::unordered_map<ChargePtr, std::vector<EdgePtr>> new_graph;
//     for (auto elem : graph)
//     {
//         ChargePtr charge = elem.first;
//         std::vector<EdgePtr> edges_in_current_elem = elem.second;
          
//         //ChargePtr new_charge = GetChargeInPoint(charge->point + Vector{i * h, j * h, k * h});

//         //EdgePtr edge = std::make_shared<Edge>(charge, new_charge, sigma);
        
//         if (!Find(charge, edges_in_current_elem) && MakeEdge(edge))
//         {
//             edges.insert(edge);
//             graph[charge].push_back(edge);
//             charges.insert(new_charge);
//             new_graph[new_charge].push_back(edge);
//         }
        
//     }
//     graph.insert(new_graph.begin(), new_graph.end());
//     iter_number_edges++;
// } 

// Graph getter
////////////////////////////////////////////
std::unordered_map<ChargePtr, std::vector<EdgePtr>> LightningTree::GetGraph() const // возвращает копию текущего состояния графа
{
    return graph;
}

void LightningTree::Info()
{
    std::cout << "Ligtning Tree info:" << '\n';
    std::cout << "Iter charges number " << iter_number_charges << '\n';
    std::cout << "Iter edges number " << iter_number_edges << '\n';
    std::cout << "Charges count: " << charges.size() << '\n';
    std::cout << "Charges in graph count: " << graph.size() << '\n';
    std::cout << "Edges count: " << edges.size() << '\n';
    std::cout << std::endl;
}
