#include "LTBuilder.h"

// template <class Tree>
// std::unique_ptr<Tree>  LTBuilder::CreateLightningTree()
// {
//     return std::make_unique<Tree>(h, delta_t, r, R, n_peripheral_layers,
//     q_plus_max, q_minus_max, Q_plus_s, 
//     Q_minus_s, resistance, E_plus, E_minus, 
//     eta, beta, sigma, phi_a, graph, edges, vertices);
// }

LTBuilder& LTBuilder::SetPeripheralLayers(size_t n_peripheral_layers_)
{
    n_peripheral_layers = n_peripheral_layers_;
    return *this;
}
    
LTBuilder& LTBuilder::SetCharge(VertexPtr vertex)
{
    vertices.insert(vertex);
    return *this;
}

LTBuilder& LTBuilder::SetEdge(EdgePtr edge)
{
    SetCharge(edge->from);
    SetCharge(edge->to);
    graph[edge->from].push_back(edge);
    graph[edge->to].push_back(edge);
    edges.insert(edge);
    return *this;
}

LTBuilder& LTBuilder::SetEdge(VertexPtr from, VertexPtr to)
{
    SetCharge(from);
    SetCharge(to);
    EdgePtr edge = std::make_shared<Edge>(from, to, sigma);  
    SetEdge(edge); 
    return *this;
}

LTBuilder& LTBuilder::SetEPlus(double E_plus_)
{
    E_plus = E_plus_;
    return *this;

}
LTBuilder& LTBuilder::SetEMinus(double E_minus_)
{
    E_minus = E_minus_;
    return *this;
}

LTBuilder& LTBuilder::SetH(double h_)
{
    h = h_;
    return *this;
}

LTBuilder& LTBuilder::Setr(double r_)
{
    r = r_;
    return *this;
}

LTBuilder& LTBuilder::SetR(double R_)
{
    R = R_;
    return *this;
}

LTBuilder& LTBuilder::SetDeltat(double delta_t_)
{
    delta_t = delta_t_;
    return *this;
}

LTBuilder& LTBuilder::SetEta(double eta_)
{
    eta = eta_;
    return *this;
}
LTBuilder& LTBuilder::SetBeta(double beta_)
{
    beta = beta_;
    return *this;
}

LTBuilder& LTBuilder::SetSigma(double sigma_)
{
    sigma = sigma_;
    return *this;
}
LTBuilder& LTBuilder::SetResistance(double resistance_)
{
    resistance = resistance_;
    return *this;
}

LTBuilder& LTBuilder::SetExternalField(std::shared_ptr<ExternalField> phi_a_)
{
    phi_a = phi_a_;
    return *this;
}

LTBuilder& LTBuilder::Countqmax()
{
    q_minus_max = 2 * pi * epsilon_0 * r * h * E_minus;
    q_plus_max = 2 * pi * epsilon_0 * r * h * E_plus;
    return *this;
}

LTBuilder& LTBuilder::CountQs()
{
    Q_plus_s = 4 * pi * epsilon_0 * R * R * E_plus;
    Q_minus_s = 4 * pi * epsilon_0 * R * R * E_minus;
    return *this;
}

LTBuilder& LTBuilder::CountDeltat()
{
    delta_t = 2 * epsilon_0 * h / (sigma * r) / 5;
    return *this;
}