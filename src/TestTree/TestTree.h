#pragma once
#include "../AbstractTree/AbstractTree.h"


class TestTree : public AbstractTree
{
public:
    TestTree(double h, double delta_t, double r, double R, double n_peripheral_layers, double q_plus_max, double q_minus_max, double Q_plus_s,
    double Q_minus_s, double resistance,
    double E_plus, double E_minus, double eta, double beta, double sigma, std::shared_ptr<ExternalField> phi_a, 
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<VertexPtr> vertices): AbstractTree(h, delta_t, r, R, n_peripheral_layers,
        q_plus_max, q_minus_max, Q_plus_s, 
        Q_minus_s, resistance, E_plus, E_minus, 
        eta, beta, sigma, phi_a, graph, edges, vertices)
    {

    }
    void NextIterEdges() override
    {
        return;
    }  
    bool IsAbleToGrow(VertexPtr vertex) override
    {
        return false;
    }
    bool MakeEdge(EdgePtr edge) override
    {
        return false;
    }
};