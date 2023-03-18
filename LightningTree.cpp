#include "LightningTree.h"


std::mt19937 gen(42);
std::uniform_real_distribution<> dis(0, 1);

// Lightning Tree
/////////////////

LightningTree::LightningTree(double h, double delta_t, double r, double R, double n_peripheral_layers, double q_plus_max, double q_minus_max, double Q_plus_s,
    double Q_minus_s, double resistance,
    double E_plus, double E_minus, double eta, double beta, double sigma, std::shared_ptr<ExternalField> phi_a, 
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<VertexPtr> vertices) : AbstractTree(h, delta_t, r, R, n_peripheral_layers,
        q_plus_max, q_minus_max, Q_plus_s, 
        Q_minus_s, resistance, E_plus, E_minus, 
        eta, beta, sigma, phi_a, graph, edges, vertices) {
        };
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
        return (1 - std::exp(-std::pow(((E - E_plus)/ E_plus), 2))) > probability;
    }
    else if (-E > E_minus)
    {
        // std::cout << E << ' ' << (1 - std::exp(-std::pow((E / E_plus), 2.5))) << std::endl;
        return (1 - std::exp(-std::pow(((-E - E_minus)/ E_minus), 2))) > probability;
    }
    
    return false;
}

bool LightningTree::IsAbleToGrow(VertexPtr vertex)
{
    return vertex->Q > Q_plus_s || -vertex->Q > Q_minus_s;
}

// Main Logic Functions
/////////////////////////////////////////////
void LightningTree::NextIterEdges() // count new edges using dis
{
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> new_graph;
    for (auto elem : graph)
    {
        VertexPtr vertex = elem.first;
        // std::vector<EdgePtr> edges_in_current_elem = elem.second;
        if (IsAbleToGrow(vertex))
        {
        
            for (int i = -1; i < 2; ++i)
            {
                for (int j = -1; j < 2; ++j)
                {
                    for (int k = -1; k < 2; ++k)
                    {
                        
                        bool find_in_point = false;
                        Vector point = vertex->point + Vector{i * h, j * h, k * h};
                        if (FindInTree(point))
                        {
                            continue;
                        }
                        auto new_vertex = CreateChargeInPoint(point);
                        if (auto charge_in_point = FindChargeInPoint(point); charge_in_point.has_value())
                        {
                            find_in_point = true;
                            new_vertex = *charge_in_point;
                        }
                        if (EdgePtr edge = std::make_shared<Edge>(vertex, new_vertex, sigma); MakeEdge(edge))
                        {
                            edges.insert(edge);
                            graph[vertex].push_back(edge);
                            if (!find_in_point)
                            {
                                vertices.insert(new_vertex);
                            }
                            DeleteFromPerephery(vertex);
                            new_graph[new_vertex].push_back(edge);
                            peripheral[new_vertex] = 0;    
                        }
                    }
                }
            }
        }
    }
    graph.insert(new_graph.begin(), new_graph.end());
}


