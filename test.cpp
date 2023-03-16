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

    auto h = 10;
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
                    .Setr(0.1)
                    .SetR(10)
                    .SetEdge(std::make_shared<Vertex>(Vector{0, 0, 8100 - h}, 0, 0), std::make_shared<Vertex>(Vector{0, 0, 8000}, 0, 0))
                    .CountDeltat()
                    .Countqmax()
                    .CountQs()
                    .CreateLightningTree<LightningTree>();

    
    
    int n_iter = 10000;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < n_iter; ++i)
    {
        lt->NextIter();
        WriteInFile<LightningTree>(*lt);
        // lt->Info();
        // PrintCurrentState(*lt);
        // std::cout << '\n';
    }
    auto end = std::chrono::system_clock::now();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << '\n'; 
    lt->Info();
    // PrintCurrentState(*lt);
    lt->ReturnFiles("vertex_table.txt", "edge_table.txt", "q_history.txt", "Q_history.txt");
    return 0;
}