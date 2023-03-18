#pragma once
#include "Vector.h"
#include <vector>
#include <memory>

struct Vertex
{
    Vector point;
    double q;
    double Q;
    std::vector<double> q_history;
    std::vector<double> Q_history;
    inline void Memorize()
    {
        q_history.push_back(q);
        Q_history.push_back(Q);
    }

    inline Vertex GetMirror()
    {
        return Vertex{Vector{point.x(), point.y(), -point.z()},-q, -Q};
    }

    std::string tostring() const
    {
        return {point.tostring() + ", " + std::to_string(Q) + ", " + std::to_string(q)};
    }
};

using VertexPtr = std::shared_ptr<Vertex>;

inline std::ostream& operator<<(std::ostream& out, const Vertex& rhs)
{
    return out << ' ' << rhs.q << ' ' << rhs.Q << ' ' << rhs.point;
}

inline double Potential(VertexPtr vertex, const Vector& point, double h)
{
    if (Abs(vertex->point - point) < kEps)
    {
        return vertex->q / (4 * pi * epsilon_0 * (h/2));
    }
    return vertex->q / (4 * pi * epsilon_0 * (Abs(vertex->point - point)));
}

inline double ShealthPotential(VertexPtr charge, const Vector& point, double R)
{
    if (Abs(charge->point - point) < kEps)
    {
        return charge->Q / (4 * pi * epsilon_0 * (R/2));
    }
    return charge->Q / (4 * pi * epsilon_0 * (Abs(charge->point - point)));
}

template <class Sequence>
inline double Potential(const Sequence& vertices, const Vector& point, double h)
{
    double ans = 0;
    for (auto& elem: vertices)
    {
        ans += Potential(elem, point, h) + Potential(std::make_shared<Vertex>(elem->GetMirror()), point, h);
    }
    return ans;
}

template <class Sequence>
inline double ShealthPotential(const Sequence& vertices, const Vector& point, double R)
{
    double ans = 0;
    for (auto& elem: vertices)
    {
        ans += ShealthPotential(elem, point, R) + ShealthPotential(std::make_shared<Vertex>(elem->GetMirror()), point, R);
    }
    return ans;
}