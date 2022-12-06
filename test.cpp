#include "LTBuilder.h"
#include "IOTree.h"
#include "LightningTree.cpp"
#include "LTBuilder.cpp"
//#include <iostream>

int main(){
    std::unordered_map<std::string, double> params = {{"electricity", 0.1}};
    ExternalField ef(FieldType::constField, params);
    LightningTree lt =  LTBuilder()
                            .SetResistance(0)
                            .SetExternalField(ef)
                            .SetEdgeInTheMiddle(0)
                            .CreateLightningTree();
    
    //Charge from = {{100, 100, 100}, 1e-5, 1e-10};
    //Charge to = {{110, 110, 110}, 2e-7, 2e-14};
    //lt.set_edge(from, to, 0.1);
    
    int n_iter = 2;
    for (int i = 0; i < n_iter; ++i)
    {
        lt.NextIter();
        // WriteInFile(lt);
        lt.Info();
    }
    WriteInFile(lt);

    // auto graph = lt.GetGraph();

    // auto el = graph.begin()->first;
    // std::cout << "Заряд вершины: " << el.q <<std::endl;
    // std::cout << "Заряд чехла: " << el.Q << std::endl << "Координаты вершины: ";
    // double charge_sum = 0;
    // for(auto elem: graph){
    //     charge_sum += elem.first->q + elem.first->Q;
    //     std::cout << elem.first->point << ' ' << elem.first->q << ' ' << elem.first->Q << ' ' << std::endl;
    // }
    // std::cout << "Charge sum: " << charge_sum << std::endl;

    //std::cout << "Hello World!";

    return 0;
}