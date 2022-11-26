#pragma once
#include "LightningTree.h"
#include <fstream>
#include <iostream>

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
    LightningTree* tree = ParseTree(fin);
    */

    }
    return {/*tree*/};
}

void WriteInFile(const LightningTree& tree)
{
    ofstream fout("LightningTree.txt"); 
    //fout << ParseToString(tree);
    fout.close();
}