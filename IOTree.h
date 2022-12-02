#pragma once
#include "LightningTree.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void ParseTree(ifstream fin, LightningTree*);

string TreeToString(const LightningTree&);

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
    ifstream fin(filename, ios_base::in);
    LightningTree* tree = new LightningTree();
    if (!fin.is_open()) // если файл не открыт
        cout << "Файл не может быть открыт!\n"; // сообщить об этом
    else
    {

    /// char buff[50]; // буфер промежуточного хранения считываемого из файла текста
    /// fin >> buff; // считали первое слово из файла
    /// fin.getline(buff, 50); // считали строку из файла
    /// fin.close(); // закрываем файл

    /*
    ParseTree(fin, tree);
    */

    }
    return {/*tree*/};
}

void WriteInFile(const LightningTree& tree)
{
    ofstream fout("LightningTree.txt", ios_base::out); 
    fout << TreeToString(tree);
    fout.close();
}

void ParseTree(ifstream fin, LightningTree* tree)
{
    
}

std::string TreeToString(const LightningTree& tree)
{
    string outtree = "";
    //string outcharges;
    for(auto elem: tree.GetGraph()){
        ///outtree += "" + *elem.first ;
        for(int i = 0; i < elem.second.size(); i++)
        {
            outtree += "\n" + elem.second[i]->from->tostring() + "\n" + elem.second[i]->to->tostring() + "\n" + std::to_string(elem.second[i]->sigma) + "\n";
        }
    }

    return outtree;
}