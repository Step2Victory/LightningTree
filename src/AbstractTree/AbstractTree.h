#pragma once
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include "../Vertex.h"
#include "../Edge.h"
#include "../ExternalField.h"
#include <fstream>
#include "../config.h"
#include "../LightningTree/MaxPotentialEdge.h"

class AbstractTree
{
    friend class MaxPotentialEdgeRange;
protected:
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph;
    std::unordered_set<EdgePtr> edges;
    std::unordered_set<VertexPtr> vertices;
    
    uint64_t iter_number = 0;

    double h;
    double delta_t;
    double r;
    double R;
    double q_plus_max;
    double q_minus_max;
    double Q_plus_s;
    double Q_minus_s;
    double resistance;
    double E_plus;
    double E_minus;
    double alpha;
    double beta;
    double sigma;
    size_t n_periferal_layers;
    std::shared_ptr<ExternalField> phi_a;
    std::unordered_map<VertexPtr, size_t> peripheral;

    void Memorize();

    double ElectricFieldAlongEdge(EdgePtr edge);
    double CurrentAlongEdge(EdgePtr edge);
    double ElectricFieldAlongEdgeRad(EdgePtr edge);
    double Heaviside(double x);
    double CurrentSheath(VertexPtr vertex);

    void DeleteCharge(VertexPtr vertex);
    void DeleteFromPerephery(VertexPtr vertex);
    std::optional<VertexPtr> FindChargeInPoint(const Vector& point);
    void DeletePeripheral();

    bool FindInTree(const Vector& point);
    bool FindInGivenEdges(const Vector& point, const std::vector<EdgePtr>& edges);

    VertexPtr CreateChargeInPoint(const Vector& point);
    
    void NextIterCharges();

public:
    AbstractTree(double h, double delta_t, double r, double R, double n_peripheral_layers, double q_plus_max, double q_minus_max, double Q_plus_s,
    double Q_minus_s, double resistance,
    double E_plus, double E_minus, double alpha, double beta, double sigma, std::shared_ptr<ExternalField> phi_a, 
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> graph, std::unordered_set<EdgePtr> edges, std::unordered_set<VertexPtr> vertices);
    


    virtual void NextIter();
    virtual void NextIterSigma();
    virtual void NextIterEdges() = 0;
    virtual bool IsAbleToGrow(VertexPtr vertex) = 0;
    virtual bool MakeEdge(EdgePtr edge) = 0;

    virtual double GetDeltaT() const;
    virtual int GetIterNumber() const;

    void Info();
    void ParamsInfo();
    
    std::unordered_map<VertexPtr, std::vector<EdgePtr>> GetGraph() const; // возвращает копию текущего состояния графа

    void ReturnFiles(const std::filesystem::path& table_vertex, const std::filesystem::path& table_edges, const std::filesystem::path& table_q_history, const std::filesystem::path& table_Q_history);
    void SavePhiInfo(const std::filesystem::path& phi_info, double start_x, double start_y, double start_z, double end_x, double end_y, double end_z);

    inline virtual ~AbstractTree() {};
};

// template <class Tree>
// std::string TreeToString(const Tree& tree)
// {
//     std::string outtree = "";
//     //string outcharges;
//     for(auto elem: tree.GetGraph()){
//         ///outtree += "" + *elem.first ;
//         for(int i = 0; i < elem.second.size(); i++)
//         {
//             outtree += "" + elem.second[i]->from->tostring() + "; " + elem.second[i]->to->tostring() + "; " + std::to_string(elem.second[i]->sigma) + "\n";
//         }
//     }

//     return outtree;
// }

// template <class Tree>
// void WriteInFile(const Tree& tree)
// {
//     std::ofstream fout(path_data + "/LightningTree.txt", std::ios_base::out);
//     fout << TreeToString(tree);
//     fout.close();
// }


