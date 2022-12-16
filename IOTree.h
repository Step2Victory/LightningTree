#pragma once
#include "LightningTree.h"
#include "LTBuilder.h"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

void ParseTree(std::ifstream fin, LightningTree*);

std::string TreeToString(const LightningTree&);

std::string Vector::tostring() const
{
    return {"(" + std::to_string(data[0]) + ", " + std::to_string(data[1]) + ", " + std::to_string(data[2]) + ")"};
}

std::string Charge::tostring() const
{
    return {point.tostring() + ", " + std::to_string(Q) + ", " + std::to_string(q)};
}

LightningTree GetLightningTreeFromFile(std::string filename)
{
    std::ifstream fin(filename, std::ios_base::in);
    // LightningTree tree = new LightningTree();
    LTBuilder ltbuilder;
    if (!fin.is_open()) // если файл не открыт
        std::cout << "Файл не может быть открыт!\n"; // сообщить об этом
    else
    {
    /// fill ltbuilder 

    /// char buff[50]; // буфер промежуточного хранения считываемого из файла текста
    /// fin >> buff; // считали первое слово из файла
    /// fin.getline(buff, 50); // считали строку из файла
    /// fin.close(); // закрываем файл

    /*
    ParseTree(fin, tree);
    */

    }
    return ltbuilder.CreateLightningTree();
}

void WriteInFile(const LightningTree& tree)
{
    std::ofstream fout("D:/Documents/GitHub/LightningTree/LightningTree.txt", std::ios_base::out);
    fout << TreeToString(tree);
    fout.close();
}

void ParseTree(std::ifstream fin, LightningTree* tree)
{
    
}

std::string TreeToString(const LightningTree& tree)
{
    std::string outtree = "";
    //string outcharges;
    for(auto elem: tree.GetGraph()){
        ///outtree += "" + *elem.first ;
        for(int i = 0; i < elem.second.size(); i++)
        {
            outtree += "" + elem.second[i]->from->tostring() + "; " + elem.second[i]->to->tostring() + "; " + std::to_string(elem.second[i]->sigma) + "\n";
        }
    }

    return outtree;
}
