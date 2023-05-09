#include "AbstractTree.h"
#include <algorithm>
#include <fstream>
#include <chrono>

AbstractTree::AbstractTree(double h, double delta_t, double r, double R, double n_peripheral_layers, double q_plus_max, double q_minus_max, double Q_plus_s,
    double Q_minus_s, double resistance,
    double E_plus, double E_minus, double alpha, double beta, double sigma, std::shared_ptr<ExternalField> phi_a, 
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<VertexPtr> vertices) :
    graph(graph), edges(edges), vertices(vertices), h(h), delta_t(delta_t), r(r), R(R), q_plus_max(q_plus_max), q_minus_max(q_minus_max), 
    Q_plus_s(Q_plus_s), Q_minus_s(Q_minus_s), resistance(resistance), E_plus(E_plus), E_minus(E_minus), alpha(alpha), beta(beta), sigma(sigma),
    phi_a(phi_a), n_periferal_layers(n_peripheral_layers) {}; 

void AbstractTree::NextIter() // combine charges and edges count
{
    int n = 10e-3 / delta_t;
    NextIterCharges();
    NextIterSigma();
    if (iter_number % n == 0)
    {
        std::cout << iter_number * delta_t << std::endl;
        NextIterEdges();
    }
    // DeletePeripheral();
    // Memorize();
}

void AbstractTree::Memorize()
{
    for (auto& vertex: vertices)
    {
        vertex->Memorize();
    }
}

double AbstractTree::ElectricFieldAlongEdge(EdgePtr edge) // реализация формулы (7) из Leaders.pdf
{
    double l = Abs(edge->from->point - edge->to->point);
    // double l = h;
    double phi_1 = Potential(vertices, edge->from->point, h) + ShealthPotential(vertices, edge->from->point, R) + phi_a->getValue(edge->from->point); // формула (6)
    double phi_2 = Potential(vertices, edge->to->point, h) + ShealthPotential(vertices, edge->to->point,  R) + phi_a->getValue(edge->to->point);
    if (std::isinf((phi_1 - phi_2) / l))
    {
        throw std::runtime_error{"Electric field along edge is infinity!"};
    }
    return (phi_1 - phi_2) / l;
}

double AbstractTree::CurrentAlongEdge(EdgePtr edge) // реализация формулы (8) из Leaders.pdf
{
    return pi * r * r * edge->sigma * ElectricFieldAlongEdge(edge);
}

double AbstractTree::ElectricFieldAlongEdgeRad(EdgePtr edge)
{
    return 0;
}

double AbstractTree::Heaviside(double x)
{
    if (x > 0)
    {
        return 1;
    }
    return 0;
}

double AbstractTree::CurrentSheath(VertexPtr vertex) // реализация формулы (10) из Leaders.pdf
{
    if (vertex->q > q_plus_max)
    {
        return (std::abs(vertex->q) - q_plus_max) / delta_t;
    }
    if (-vertex->q > q_minus_max)
    {
        return -(std::abs(vertex->q) - q_minus_max) / delta_t;
    }
    return 0;
}

void AbstractTree::DeleteCharge(VertexPtr charge) // удаляет заряд и все его ребра из дерева
{
    if (graph.contains(charge))
    {
        for (auto& edge: graph[charge])
        {
            if (edge->from == charge)
            {
                auto iter_in_to = std::find(graph[edge->to].begin(), graph[edge->to].end(), edge);
                if (iter_in_to != graph[edge->to].end())
                    graph[edge->to].erase(iter_in_to);
            
                
                if (graph[edge->to].size() == 1)
                {
                    peripheral[edge->to] = 0;;
                }
            }
            else
            {
                auto iter_in_from = std::find(graph[edge->from].begin(), graph[edge->from].end(), edge);
                graph[edge->from].erase(iter_in_from);
                if (graph[edge->from].size() == 1)
                {
                    peripheral[edge->from] = 0;
                }
            }
            edges.erase(edge);
        }
        graph.erase(charge);
    }
}

void AbstractTree::DeleteFromPerephery(VertexPtr charge)
{
    peripheral.erase(charge);
}

void AbstractTree::DeletePeripheral() // delete peripherical edges
{
    
    std::vector<VertexPtr> to_delete;
    for (auto& elem: peripheral)
    {
        elem.second++;
        if (elem.second > n_periferal_layers)
        {
            to_delete.push_back(elem.first);
        }
    }
    for (auto& elem: to_delete)
    {
        peripheral.erase(elem);
        DeleteCharge(elem);
    }
}

std::optional<VertexPtr> AbstractTree::FindChargeInPoint(const Vector& point) // возвращает заряд в точке point, если такого нет, возвращает nullopt
{
    for (auto& vertex: vertices)
    {
        if (Abs(vertex->point - point) < kEps)
        {
            return vertex;
        }
    }
    return std::nullopt;
}

bool AbstractTree::FindInTree(const Vector& point)
{
    for (auto& edge : edges)
    {
        if (Abs(edge->to->point - point) < kEps || Abs(edge->from->point - point) < kEps)
        {
            return true;
        }
    }
    return false;
}

bool AbstractTree::FindInGivenEdges(const Vector& point, const std::vector<EdgePtr>& edges) // проверяет является заряд в точке point концом какого-нибудь ребра из edges
{
    for (auto& edge : edges)
    {
        if (Abs(edge->to->point - point) < kEps || Abs(edge->from->point - point) < kEps)
        {
            return true;
        }
    }
    return false;
}

VertexPtr AbstractTree::CreateChargeInPoint(const Vector& point) // создает заряж в точке point
{
    for (auto& vertex: vertices)
    {
        if (Abs(vertex->point - point) < kEps)
        {
            return nullptr;
        }
    }
    return std::make_shared<Vertex>(point, 0, 0);
}

EdgePtr test = nullptr;
void AbstractTree::NextIterSigma()
{
    for (auto& edge: edges)
    {
        if (!test)
        {
            test = edge;
        }
        
        auto E = ElectricFieldAlongEdge(edge);
        if (edge == test && iter_number % 1000 == 1)
        {
            std::cout << edge->sigma << " " << E << " " << edge->from->Q << ' ' << edge->to->Q << std::endl;
        }
        // auto delta_sigma = (alpha * E * E - beta) * edge->sigma * delta_t;
        edge->sigma = edge->sigma * std::exp((alpha * E * E - beta) * delta_t);

        if (std::isinf(edge->sigma))
        {
            throw std::runtime_error{"Sigma is infinity!"};
        }
    }
}

void AbstractTree::NextIterCharges() // count new charges
{
    std::unordered_map<VertexPtr, std::pair<double, double>> delta_charges;
    for (auto& elem : graph)
    {
        VertexPtr vertex = elem.first;
        std::vector<EdgePtr> edges = elem.second;
        for (auto& edge: edges)
        {
            if (edge->from == vertex)
            {
                delta_charges[vertex].first -= CurrentAlongEdge(edge);
            }
            else
            {
                delta_charges[vertex].first += CurrentAlongEdge(edge);
            }
        }
        delta_charges[vertex].first -= CurrentSheath(vertex);
        delta_charges[vertex].second += CurrentSheath(vertex);
    }
    for (auto& elem : delta_charges)
    {
        elem.first->q += delta_charges[elem.first].first * delta_t;
        elem.first->Q += delta_charges[elem.first].second * delta_t;
    }
    iter_number++;
}

std::unordered_map<VertexPtr, std::vector<EdgePtr>> AbstractTree::GetGraph() const // возвращает копию текущего состояния графа
{
    return graph;
}

void AbstractTree::ParamsInfo()
{ 
    std::cout << "Lightning Tree params info:" << '\n';
    std::cout << "delta_t " << delta_t << '\n';
    std::cout << "q_minus_max " << q_minus_max << '\n';
    std::cout << "q_plus_max " << q_plus_max << '\n';
    std::cout << "Q_minus_s " << Q_minus_s << '\n';
    std::cout << "Q_plus_s " << Q_plus_s << '\n';
    std::cout << std::endl;
}

void AbstractTree::Info()
{
    
    std::cout << "Ligtning Tree info:" << '\n';
    std::cout << "Iter number " << iter_number << '\n';
    std::cout << "Vertices count: " << vertices.size() << '\n';
    std::cout << "Vertices in graph count: " << graph.size() << '\n';
    std::cout << "Edges count: " << edges.size() << '\n';
    std::cout << '\n';
}

void AbstractTree::ReturnFiles(const std::filesystem::path& table_vertex, const std::filesystem::path& table_edges, const std::filesystem::path& table_q_history, const std::filesystem::path& table_Q_history)
{
    std::ofstream fout(table_vertex);
    fout << "id" << ' ' << 'q' << ' ' << 'Q' << ' ' << 'x' << ' ' << 'y' << ' ' << 'z'  << ' ' << "phi" << '\n';
    for (auto& vertex: vertices)
    {
        fout << vertex << ' ' << *vertex << ' ' <<  Potential(vertices, vertex->point, h) + ShealthPotential(vertices, vertex->point, R) + phi_a->getValue(vertex->point) << '\n';
    }
    fout.close();
    fout.open(table_edges);
    fout << "id" << ' ' << "from" << ' ' << "to" << ' ' << "current" << ' ' << "sigma" <<  '\n';
    for (auto edge: edges)
    {
        fout << edge << ' ' << *edge << ' ' << std::abs(CurrentAlongEdge(edge))<< ' ' << edge->sigma << '\n';
    }
    fout.close();
    fout.open(table_q_history);
    for (auto& vertex: vertices)
    {
        fout << vertex << ' ';
        for (auto& elem: vertex->q_history)
        {
            fout << elem << ' ';
        }
        fout << '\n';
    }
    fout.close();
    fout.open(table_Q_history);
    for (auto& vertex: vertices)
    {
        fout << vertex << ' ';
        for (auto& elem: vertex->Q_history)
        {
            fout << elem << ' ';
        }
        fout << '\n';
    }
    fout.close();

}

void AbstractTree::SavePhiInfo(const std::filesystem::path& phi_info, double start_x, double start_y, double start_z, double end_x, double end_y, double end_z)
{
    std::ofstream fout(phi_info);
    fout << 'z' << ' ' <<"full_phi" << ' ' << "ext_phi" << '\n';
    for (double z = start_z; z < end_z; z += h)
    {
        double full_sum = 0;
        double ext_sum = 0;
        for (double x = start_x; x < end_x; x += h)
        {
            for (double y = start_y; y < end_y; y += h)
            {
                full_sum += Potential(vertices, Vector{x, y, z}, h) + ShealthPotential(vertices, Vector{x, y, z}, R) + phi_a->getValue(Vector{x, y, z});
                ext_sum += phi_a->getValue(Vector{x, y, z});
            }
        }
        fout << z << ' ' << full_sum * (h * h) / ((end_x - start_x) * (end_y - start_y)) << ' ' << ext_sum * (h * h) / ((end_x - start_x) * (end_y - start_y)) << '\n';
    }
    fout.close();
}