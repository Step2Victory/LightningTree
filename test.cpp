#include "LTBuilder.h"
#include "LightningTree.h"
#include <chrono>


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

int main(){
    // auto field = std::make_shared<ConstField>(300000);
    auto field = std::make_shared<NormalField>(2000, 8000, 335'000'000);

    double h = 10;
    std::shared_ptr<ExternalField> ef = field;
    auto lt =  LTBuilder()
                    .SetPeripheralLayers(1000000)
                    .SetResistance(1)
                    .SetExternalField(ef)
                    .SetEPlus(150000)
                    .SetEMinus(300000)
                    .SetE0(100000)
                    .SetEb(3000000)
                    .SetSigma(1)
                    .SetH(h)
                    .Setr(h / 100)
                    .SetR(h)
                    .SetEdge(std::make_shared<Vertex>(Vector{0, 0, 8100.0 - h}, 0, 0), std::make_shared<Vertex>(Vector{0, 0, 8000}, 0, 0))
                    .CountDeltat()
                    .Countqmax()
                    .CountQs()
                    .CreateLightningTree<LightningTree>();

    
    
    int n_iter = 10000;
    auto start = std::chrono::system_clock::now();
    lt->ParamsInfo();
    for (int i = 0; i < n_iter; ++i)
    {
        lt->NextIter();
        WriteInFile<LightningTree>(*lt);
    }
    auto end = std::chrono::system_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << '\n'; 
    lt->Info();
    // PrintCurrentState(*lt);
    lt->ReturnFiles("data/vertex_table.txt", "data/edge_table.txt", "data/q_history.txt", "data/Q_history.txt");
    return 0;
}