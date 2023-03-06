#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "Vertex.h"
#include "Edge.h"
#include "ExternalField.h"

class LTBuilder
{
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph;
    std::unordered_set<EdgePtr> edges;
    std::unordered_set<VertexPtr> vertices;
    
    uint64_t iter_number = 0;

    double h = 100;
    double delta_t;
    double r = 0.01;
    double R = 50;
    double q_plus_max;
    double q_minus_max;
    double Q_plus_s;
    double Q_minus_s;
    double resistance = 5;
    double E_plus = 50;
    double E_minus = 100;
    double eta = 0;
    double beta = 0;
    double sigma = 1;
    size_t n_peripheral_layers;
    std::shared_ptr<ExternalField> phi_a;

public:
    LTBuilder() = default;
    // template <class Tree>
    template<class Tree>
    inline std::unique_ptr<Tree> CreateLightningTree()
    {
            return std::make_unique<Tree>(h, delta_t, r, R, n_peripheral_layers,
        q_plus_max, q_minus_max, Q_plus_s, 
        Q_minus_s, resistance, E_plus, E_minus, 
        eta, beta, sigma, phi_a, graph, edges, vertices);
    }

    LTBuilder& SetPeripheralLayers(size_t n_peripheral_layers);
    
    LTBuilder& SetCharge(VertexPtr charge);
    LTBuilder& SetEdge(EdgePtr edge);
    LTBuilder& SetEdge(VertexPtr from, VertexPtr to);

    LTBuilder& SetEPlus(double E_plus);
    LTBuilder& SetEMinus(double E_minus);

    LTBuilder& SetH(double h);
    LTBuilder& Setr(double r);
    LTBuilder& SetR(double R);
    
    LTBuilder& SetDeltat(double delta_t);

    LTBuilder& SetEta(double eta);
    LTBuilder& SetBeta(double beta);

    LTBuilder& SetSigma(double sigma);
    LTBuilder& SetResistance(double resistance);

    LTBuilder& SetExternalField(std::shared_ptr<ExternalField> phi_a);
    LTBuilder& SetEdgeInTheMiddle(double q); // добавляет два заряда и ребро в центр расчетной области

    LTBuilder& Countqmax();
    LTBuilder& CountQs();
    LTBuilder& CountDeltat();

};