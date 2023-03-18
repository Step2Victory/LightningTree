#include "WormTree.h"


void WormTree::NextIterEdges()
{
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> new_graph;
    for (auto elem : graph)
    {
        VertexPtr vertex = elem.first;
        if (IsAbleToGrow(vertex))
        {
        
            for (int i = -1; i < 2; ++i)
            {        
                bool find_in_point = false;
                Vector point = vertex->point + Vector{0, 0, i * h};
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
                    peripheral[0].push_back(new_vertex);    
                }
            }
        }
    }
    graph.insert(new_graph.begin(), new_graph.end());
    
}

bool WormTree::IsAbleToGrow(VertexPtr vertex)
{
    return vertex->Q > q_plus_max || -vertex->Q > q_minus_max;
}

bool WormTree::MakeEdge(EdgePtr edge)
{
    return true;
}