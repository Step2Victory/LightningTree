#pragma once
#include "AbstractTree.h"


class WormTree : public AbstractTree
{
public:
    WormTree(double h, double delta_t, double r, double R, double n_peripheral_layers, double q_plus_max, double q_minus_max, double Q_plus_s,
    double Q_minus_s, double resistance,
    double E_plus, double E_minus, double eta, double beta, double sigma, std::shared_ptr<ExternalField> phi_a, 
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<VertexPtr> vertices);

    virtual void NextIterEdges();
    virtual bool IsAbleToGrow(VertexPtr vertex);
    virtual bool MakeEdge(EdgePtr edge);

};
