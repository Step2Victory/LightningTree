#include "LightningTree.h"
#include "IOTree.h"
//#include <iostream>

int main(){
    //std::map<std::string, double> params = {{"a", 0}};
    ExternalField ef = ExternalField();
    LightningTree lt = {5, 1, 1, 1, 10, 10, 10, 0.1, 1, 0.01, 0.1, 1, -1, 150'000, 1, 1, 1, 1, ef};

    lt.NextIter();
    
    auto graph = lt.GetGraph();
    
    for(auto elem: graph){

        std::cout << (*elem.first).point<<std::endl;
    }

    return 0;
}