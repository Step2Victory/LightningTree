#include "src/LTBuilder.h"
#include "src/LightningTree/LightningTree.h"
#include "src/TestTree/TestTree.h"
#include <chrono>
#include <filesystem>
#include "src/config.h"


void PrintCurrentState(const LightningTree& lt)
{
    auto graph = lt.GetGraph();
    double charge_sum = 0;
    for(auto elem: graph){
        charge_sum += elem.first->q + elem.first->Q;
        std::cout << elem.first->point << ' ' << elem.first->q << ' ' << elem.first->Q << ' ' << '\n';
    }
    std::cout << "Charge sum: " << charge_sum << '\n';
}


void GenerateSessions()
{
    double h = 30, E_plus = 150000;
    int per = 500;
    auto field = std::make_shared<NormalField>(2000, 8000, 335'000'000);
    field->DeduceMult(E_plus, h);
    std::shared_ptr<ExternalField> ef = field;
    std::string dir_name =  "examples_per" + std::to_string(per) + "_EminusE_plus_pow2";
    std::filesystem::create_directory(current_path / dir_name);
    auto path = current_path / dir_name;
    std::vector numbers_of_iters = {100, 500, 1000, 2000, 3000, 3500, 5000};
    for (auto n_iters: numbers_of_iters)
    {
    
        auto lt =  LTBuilder()
                        .SetPeripheralLayers(per)
                        .SetResistance(1)
                        .SetExternalField(ef)
                        .SetEPlus(E_plus)
                        .SetEMinus(2 * E_plus)
                        .SetE0(100000)
                        .SetEb(3000000)
                        .SetSigma(1)
                        .SetH(h)
                        .Setr(h / 100)
                        .SetR(h)
                        .SetEdge(std::make_shared<Vertex>(Vector{0, 0, 8000.0}, 0, 0), std::make_shared<Vertex>(Vector{0, 0, 8000 + h}, 0, 0))
                        .CountDeltat()
                        .Countqmax()
                        .CountQs()
                        .CreateLightningTree<LightningTree>();
        for (int i = 0; i < n_iters; ++i)
        {
            lt->NextIter();
            lt->Info();
        }
        
        auto dir_path = path / std::to_string(n_iters);
        std::filesystem::create_directory(dir_path);
        lt->ReturnFiles(
            dir_path / "vertex_table.txt", 
            dir_path / "edge_table.txt", 
            dir_path / "q_history_1.txt", 
            dir_path / "Q_history.txt"
            );
    }
    
    
}

int main(){
    // auto field = std::make_shared<ConstField>(300000);
    

    double h = 30, E_plus = 150000;
    auto field = std::make_shared<NormalField>(2000, 8000, 335'000'000);
    field->DeduceMult(E_plus, h);
    std::shared_ptr<ExternalField> ef = field;
    auto lt =  LTBuilder()
                    .SetPeripheralLayers(220)
                    .SetResistance(1)
                    .SetExternalField(ef)
                    .SetEPlus(E_plus)
                    .SetEMinus(2 * E_plus)
                    .SetE0(100000)
                    .SetEb(3000000)
                    .SetSigma(1)
                    .SetH(h)
                    .Setr(h / 1000)
                    .SetR(h)
                    .SetEdge(std::make_shared<Vertex>(Vector{0, 0, 8000.0}, 0, 0), std::make_shared<Vertex>(Vector{0, 0, 8000 + h}, 0, 0))
                    .CountDeltat()
                    .Countqmax()
                    .CountQs()
                    .CreateLightningTree<LightningTree>();
    
    
    // auto ltb =  LTBuilder()
    //             .SetPeripheralLayers(1000000)
    //             .SetResistance(1)
    //             .SetExternalField(ef)
    //             .SetEPlus(E_plus)
    //             .SetEMinus(2 * E_plus)
    //             .SetE0(100000)
    //             .SetEb(3000000)
    //             .SetSigma(1)
    //             .SetH(h)
    //             .Setr(h / 100)
    //             .SetR(h)
    //             .CountDeltat()
    //             .Countqmax()
    //             .CountQs();
    //             // .CreateLightningTree<TestTree>();

    // auto k = 6;
    // std::vector<VertexPtr> vertices(k);
    // for (size_t i = 0; i < k; ++i)
    // {
    //     vertices[i] = std::make_shared<Vertex>(Vector{0, 0, 8000.0 + i * h}, 0, 0);
    // }
    // for (size_t i = 1; i < k; ++i)
    // {
    //     ltb.SetEdge(vertices[i - 1], vertices[i]);
    // }
    // auto lt = ltb.CreateLightningTree<TestTree>();

    // GenerateSessions();
    int n_iter = 2000;
    auto start = std::chrono::system_clock::now();
    lt->ParamsInfo();
    for (int i = 0; i < n_iter; ++i)
    {
        lt->NextIter();
        lt->Info();
        // lt->ReturnFiles(path_data + "/vertex_table.txt", path_data + "/edge_table.txt", path_data + "/q_history_1.txt", path_data + "/Q_history.txt");
    }
    auto end = std::chrono::system_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << '\n'; 
    lt->Info();
    // // PrintCurrentState(*lt);
    // lt->ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
    return 0;
}