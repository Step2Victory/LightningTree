#pragma once
#include "LightningTree.h"

#ifndef LTBuilder_cpp 
    #define LTBuilder_cpp
    
class LTBuilder
{
    std::unordered_map<ChargePtr, std::vector<EdgePtr>> graph;
    std::unordered_set<EdgePtr> edges;
    std::unordered_set<ChargePtr> charges;
    
    uint64_t iter_number_charges = 0;
    uint64_t iter_number_edges = 0;

    double h = 100;
    double x_min = -2000;
    double y_min = -2000;
    double z_min = 7000;
    double x_max = 2000;
    double y_max = 2000;
    double z_max = 9000;
    double delta_t = 0.001;
    double delta_T = 0.001;
    double r = 1;
    double R = 50;
    double q_plus_max = 0.005;
    double q_minus_max = 0.01;
    double resistance = 5;
    double E_plus = 50;
    double E_minus = 100;
    double eta = 0;
    double beta = 0;
    double sigma = 100;
    ExternalField phi_a;

public:
    LTBuilder() = default;
    LightningTree CreateLightningTree();

    LTBuilder& SetCharge(std::shared_ptr<Charge> charge);
    LTBuilder& SetEdge(std::shared_ptr<Edge> edge);
    LTBuilder& SetEdge(std::shared_ptr<Charge> from, std::shared_ptr<Charge> to);

    LTBuilder& SetXLimits(double x_min, double x_max);
    LTBuilder& SetYLimits(double y_min, double y_max);
    LTBuilder& SetZLimits(double x_min, double x_max);

    LTBuilder& SetH(double h);
    LTBuilder& Setr(double r);
    LTBuilder& SetR(double R);
    
    LTBuilder& SetDeltat(double delta_t);
    LTBuilder& SetDeltaT(double delta_T);

    LTBuilder& SetEta(double eta);
    LTBuilder& SetBeta(double beta);

    LTBuilder& SetSigma(double sigma);
    LTBuilder& SetResistance(double resistance);

    LTBuilder& SetExternalField(const ExternalField& phi_a);
    LTBuilder& SetEdgeInTheMiddle(double q); // добавляет два заряда и ребро в центр расчетной области
};

#endif