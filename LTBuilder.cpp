#include "LTBuilder.h"

LightningTree LTBuilder::CreateLightningTree()
{
    return LightningTree(h, x_min, y_min, z_min, x_max, y_max, z_max, delta_t, delta_T, r, R, 
    q_plus_max, q_minus_max, resistance, E_plus, E_minus, eta, beta, sigma, phi_a, graph, edges, charges);
}
    
LTBuilder& LTBuilder::SetCharge(std::shared_ptr<Charge> charge)
{
    charges.insert(charge);
    return *this;
}

LTBuilder& LTBuilder::SetEdge(std::shared_ptr<Edge> edge)
{
    SetCharge(edge->from);
    SetCharge(edge->to);
    graph[edge->from].push_back(edge);
    graph[edge->to].push_back(edge);
    edges.insert(edge);
    return *this;
}

LTBuilder& LTBuilder::SetEdge(std::shared_ptr<Charge> from, std::shared_ptr<Charge> to)
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

LTBuilder& LTBuilder::SetXLimits(double x_min_, double x_max_)
{
    x_min = x_min_;
    x_max = x_max_;
    return *this;
}

LTBuilder& LTBuilder::SetYLimits(double y_min_, double y_max_)
{
    y_min = y_min_;
    y_max = y_max_;
    return *this;
}

LTBuilder& LTBuilder::SetZLimits(double z_min_, double z_max_)
{
    z_min = z_min_;
    z_max = z_max_;
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

LTBuilder& LTBuilder::SetDeltaT(double delta_T_)
{
    delta_T = delta_T_;
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

LTBuilder& LTBuilder::SetEdgeInTheMiddle(double q)
 {
    ChargePtr first = std::make_shared<Charge>(Vector{(x_max + x_min) / 2, (y_max + y_min) / 2, (z_max + z_min) / 2}, q, 0, positive);
    ChargePtr second = std::make_shared<Charge>(Vector{(x_max + x_min) / 2, (y_max + y_min) / 2, (z_max + z_min) / 2 + h}, -q, 0, negative);
    SetEdge(first, second);
    return *this;
 }