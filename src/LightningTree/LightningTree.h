#pragma once
#include <cstdlib>
#include <cmath>
#include <random>
#include <utility>
#include "../AbstractTree/AbstractTree.h"


class LightningTree : public AbstractTree
{
    friend class MaxPotentialEdgeRange;

    virtual bool MakeEdge(EdgePtr edge) override; // содержит реализацию формулы (12) из Leaders.pdf
public:
    LightningTree(const std::filesystem::path &path_to_config_file);
    LightningTree(double h, double delta_t, double r, double R, double n_peripheral_layers, double q_plus_max, double q_minus_max, double Q_plus_s,
    double Q_minus_s, double resistance,
    double E_plus, double E_minus, double alpha, double beta, double sigma, std::shared_ptr<ExternalField> phi_a, 
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<VertexPtr> vertices);
    virtual void NextIterEdges() override; // count new edges using dis 

    
    // void LightningTree::DelEdges(); // delete edges that don't create new ones
    virtual bool IsAbleToGrow(VertexPtr vertex) override;
    std::array<std::pair<double, std::array<int, 3>>, 26> GetRes(VertexPtr vertex);
    inline ~LightningTree() {};
};
