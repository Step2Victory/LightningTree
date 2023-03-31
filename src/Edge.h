#pragma once
#include <iostream>
#include "Vertex.h"

struct Edge
{
    VertexPtr from;
    VertexPtr to;
    double sigma;
};

using EdgePtr = std::shared_ptr<Edge>;

inline std::ostream& operator<<(std::ostream& out, const Edge& edge)
{
    return out << edge.from << ' ' << edge.to;
}