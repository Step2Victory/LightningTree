#pragma once
#include "LightningTree.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

LightningTree GetLightningTreeFromFile(std::string filename)
{
    ifstream fin(filename, ios_base::in);
 
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
    fout << ParseToString(tree);
    fout.close();
}

void ParseTree(ifstream fin)
{

}

string ParseToString(const LightningTree& tree)
{
    string outtree;
    string outcharges;
    for(auto elem: tree.GetGraph()){
        //outcharges += "("+elem.first->point.data[0]+", "+elem.first->point.data[1]+", "+elem.first->point.data[2]+") "+", "+elem.first->Q+", "+elem.first->q+";"+
        for(int i = 0; i < elem.second.size(); i++)
        outtree += "" + *elem.second[i]->from + "; "+ *elem.second[i]->to + "; " + elem.second[i]->sigma + ";";
    }

    return outtree;
}