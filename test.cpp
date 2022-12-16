#include "LTBuilder.h"
#include "IOTree.h"
//#include <iostream>


void PrintCurrentState(const LightningTree& lt)
{
    auto graph = lt.GetGraph();
    double charge_sum = 0;
    for(auto elem: graph){
        charge_sum += elem.first->q + elem.first->Q;
        std::cout << elem.first->point << ' ' << elem.first->q << ' ' << elem.first->Q << ' ' << std::endl;
    }
    std::cout << "Charge sum: " << charge_sum << std::endl;
}

int main(){
    std::unordered_map<std::string, double> params = {{"electricity", 200000}};
    ExternalField ef(FieldType::constField, params);
    LightningTree lt =  LTBuilder()
                            .SetResistance(0)
                            .SetExternalField(ef)
                            .SetEdgeInTheMiddle(0)
                            .SetEPlus(100000)
                            .SetEMinus(200000)
                            .SetDeltaT(0.00001)
                            .SetDeltat(0.00001)
                            .SetSigma(4.8)
                            .SetH(100)
                            .Setr(0.01)
                            .CreateLightningTree();
    
    int n_iter = 100;
    for (int i = 0; i < n_iter; ++i)
    {
        lt.NextIter();
        WriteInFile(lt);
        lt.Info();
        PrintCurrentState(lt);
        std::cout << std::endl;
    }
    WriteInFile(lt);
    return 0;
}