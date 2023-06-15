#include "src/LTBuilder.h"
#include "src/LightningTree/LightningTree.h"
#include "src/TestTree/TestTree.h"
#include <chrono>
#include <filesystem>
#include <thread>
#include "src/config.h"

using namespace std::chrono_literals;

void PrintCurrentState(const LightningTree& lt)
{
    auto graph = lt.GetGraph();
    double charge_sum = 0;
    for(auto elem: graph){
        charge_sum += elem.first->q + elem.first->Q;
        std::cout << elem.first->point << ' ' << elem.first->q << ' ' << elem.first->Q << ' ' << '\n';
    }
    std::cout << "Charge sum: " << charge_sum << std::endl;
}


int ReadResponse()
{
    int response;
    std::cin >> response;
    return response;
}

void WriteAnswer(int response, const LightningTree& tree)
{
    int iter_number = tree.GetIterNumber();
    double delta_t = tree.GetDeltaT();
    std::cout << response << ' ' << iter_number << ' ' << tree.GetGraph().size() <<  ' ' << iter_number * delta_t<< std::endl;
}

int main(){
    // auto field = std::make_shared<ConstField>(300000);
    
    auto t = std::chrono::system_clock::now();
    double h = 100, E_plus = 150000;
    double start_x = -5 * h;
    double end_x = 5 * h;
    double start_y = -5 * h;
    double end_y = 5 * h;
    double start_z = 9000 - 20 * h;
    double end_z = 9000 + 20 * h;
    std::vector layers = {
        ChargeLayer{.p_0 = -0.0000005, .h = h, .L = 200, .r=Vector{0, 0, 8000}, .a=1},
        ChargeLayer{.p_0 = 0.0000005, .h = h, .L = 200, .r=Vector{0, 0, 10000}, .a=1}};
    auto start = std::chrono::system_clock::now();
    auto field = std::make_shared<TableField>(layers, start_x, start_y, start_z, end_x, end_y, end_z, h);
    auto end = std::chrono::system_clock::now();
    // std::cout << "External field preprocessing time " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "s" << std::endl;
    // auto field = std::make_shared<NormalField>(2000, 8000, 335'000'000);
    // field->DeduceMult(E_plus, h);
    std::shared_ptr<ExternalField> ef = field;
    auto lt =  LTBuilder()
                    .SetPeripheralLayers(1)
                    .SetResistance(1)
                    .SetExternalField(ef)
                    .SetEPlus(E_plus)
                    .SetEMinus(2 * E_plus)
                    .SetE0(100000)
                    .SetEb(3000000)
                    .SetH(h)
                    .Setr(0.01)
                    .SetR(h / 2)
                    .SetAlpha(5 * 1e-8)
                    .SetBeta(10)
                    .SetSigma(1e-5)
                    .SetEdge(std::make_shared<Vertex>(Vector{0, 0, 9000.0}, 0, 0), std::make_shared<Vertex>(Vector{0, 0, 9000 + h}, 0, 0))
                    .SetSigma(1e-5)
                    .CountDeltat()
                    .Countqmax()
                    .CountQs()
                    .MultiplyDeltat(1.0 / 1000)
                    .CreateLightningTree<LightningTree>();
    


    // GenerateSessions();
    int n_iter = 700000;
    start = std::chrono::system_clock::now();
    WriteAnswer(1, *lt);
    auto response = ReadResponse();
    
    for (int i = 0; i < n_iter; ++i)
    {
        if(response == 0) return 0;
        try
        {
            lt->NextIter();

            if(i % 20000 == 0){
                lt->ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
                lt->SavePhiInfo(path_data / "phi_info.txt", start_x, start_y, start_z, end_x, end_y, end_z);

                WriteAnswer(1, *lt);
                
                response = ReadResponse();
            }
        }
        catch(const std::exception& e)
        {
            // std::cerr << e.what() << '\n';
            lt->ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
            lt->SavePhiInfo(path_data / "phi_info.txt", start_x, start_y, start_z, end_x, end_y, end_z);
            WriteAnswer(0, *lt);
            return 0;
        }
        // lt->Info();
        // lt->ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
    }
    end = std::chrono::system_clock::now();
    // std::cout << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << '\n'; 
    // lt->Info();
    // // PrintCurrentState(*lt);
    lt->ReturnFiles(path_data / "vertex_table.txt", path_data / "edge_table.txt", path_data /"q_history_1.txt", path_data /"Q_history.txt");
    lt->SavePhiInfo(path_data / "phi_info.txt", start_x, start_y, start_z, end_x, end_y, end_z);
    WriteAnswer(0, *lt);
    return 0;
}