#include "LTBuilder.h"
#include "IOTree.h"
//#include <iostream>
// #include "LightningTree.cpp"
// #include "LTBuilder.cpp"


void PrintCurrentState(const LightningTree& lt)
{
    auto graph = lt.GetGraph();
    double charge_sum = 0;
    for(auto elem: graph){
        charge_sum += elem.first->q + elem.first->Q;
        std::cout << elem.first->point << ' ' << elem.first->q << ' ' << elem.first->Q << ' ' << std::endl;
    }
    std::cout << "C3harge sum: " << charge_sum << std::endl;
}

int main(){
    auto field = std::make_shared<ConstField>(300000);
    std::shared_ptr<ExternalField> ef = field;
    LightningTree lt =  LTBuilder()
                            .SetResistance(1)
                            .SetExternalField(ef)
                            .SetEPlus(100000)
                            .SetEMinus(200000)
                            .SetDeltaT(0.00001)
                            .SetDeltat(0.000001)
                            .SetSigma(10)
                            .SetH(100)
                            .Setr(0.01)
                            .SetEdgeInTheMiddle(0)
                            .CreateLightningTree();
    
    int n_iter = 100;
    for (int i = 0; i < n_iter; ++i)
    {
        lt.NextIter();
        WriteInFile(lt);
        lt.Info();
        // PrintCurrentState(lt);
        std::cout << std::endl;
    }
    WriteInFile(lt);
    return 0;
}