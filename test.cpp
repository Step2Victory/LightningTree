#include "LightningTree.h"
#include "IOTree.h"
//#include <iostream>

int main(){
    //std::map<std::string, double> params = {{"a", 0}};
    ExternalField ef = ExternalField();
    LightningTree lt = LightningTree(5, 1, 1, 1, 10, 10, 10, 0.1, 1, 0.01, 0.1, 1, -1, 150'000, 1, 1, 1, 1, ef);
    Charge from = {{100, 100, 100}, 1e-5, 1e-10};
    Charge to = {{110, 110, 110}, 2e-7, 2e-14};
    lt.set_edge(from, to, 0.1);
    lt.NextIter();
    
    auto graph = lt.GetGraph();

    auto el = *graph.begin()->first;
    std::cout << "Заряд вершины: " << el.q <<std::endl;
    std::cout << "Заряд чехла: " << el.Q << std::endl << "Координаты вершины: ";
    
    for(auto elem: graph){
        std::cout << (*elem.first).point<<std::endl;
    }

    //std::cout << "Hello World!";

    return 0;
}