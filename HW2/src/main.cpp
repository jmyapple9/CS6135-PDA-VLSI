#include "main.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
// #include <utility>
using namespace std;

ifstream in_file;
vector<pair<int, int>> TechA; // index is LibCell number-1 (ex. MC1 will be store at TechA[0])
vector<pair<int, int>> TechB; // pair<w,t>
vector<cell *> cellArray;
vector<net *> netArray;

int NumTechs;
int NumLibCells;

die Die;

// conversion table are used to prevent mismapping of: Libcell, Cell, and Net
// R is the real id in name(eg. MC3, C8, N5)
// V is the virtual id = the position of node in array(TechA, TechB, cellArray, netArray)
unordered_map<int,int>  libRtoV;
unordered_map<int,int>  libVtoR;
unordered_map<int,int>  cellRtoV;
unordered_map<int,int>  cellVtoR;
unordered_map<int,int>  netRtoV;
unordered_map<int,int>  netVtoR;

pair<string, string> eatArg(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Input format should be the following." << endl;
        cout << "eg.  ./hw2 ../testcase/public1.txt ../output/public1.out" << endl;
        cout << "exiting..." << endl;
        exit(1);
    }
    return {argv[1], argv[2]};
}

void getIss(istringstream &iss, string line)
{
    iss.clear();
    iss.str("");
    getline(in_file, line);
    iss.str(line);
}

void check(){
    cout << "checking net Array:" << endl;
    for (int i = 1; i < netArray.size(); i++)
    {
        cout << "In net " << i << " having " << netArray[i]->numCells << " cells:" << endl;
        for(int c:netArray[i]->cells){
            cout << c << " ";
        }
        cout << endl;
    }
    cout << "\nchecking cell Array:" << endl;
    for (int i = 1; i < cellArray.size(); i++)
    {
        cout << "In cell " << i << " having " << cellArray[i]->nets.size() << " nets:" << endl;
        for(int n:cellArray[i]->nets){
            cout << n << " ";
        }
        cout << endl;
    }
}

void libcellParser(){
    istringstream iss;
    string line, s1, s2, s3;
    int t1, t2, t3;

    // NumTechs
    getIss(iss, line);
    iss >> s1 >> NumTechs;

    // Tech TA's LibCell
    getIss(iss, line);
    iss >> s1 >> s2 >> NumLibCells;

    TechA.emplace_back(make_pair(-1, -1)); // put dummy node at index 0
    for (int i = 0; i < NumLibCells; i++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> t1 >> t2;
        TechA.emplace_back(make_pair(t1, t2));
    }

    // Tech TB's LibCell, if exist
    if (NumTechs == 2)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> NumLibCells;

        TechB.emplace_back(make_pair(-1, -1)); // put dummy node at index 0
        for (int i = 0; i < NumLibCells; i++)
        {
            getIss(iss, line);
            iss >> s1 >> s2 >> t1 >> t2;
            TechB.emplace_back(make_pair(t1, t2));
        }
    }
}

void DieInfoParser(){
    istringstream iss;
    string line, s1, s2, s3;
    int t1, t2, t3;

    // eat newline
    getline(in_file, line);

    // Die info
    getIss(iss, line);
    iss >> s1 >> Die.w >> Die.h;

    getIss(iss, line);
    iss >> s1 >> s2 >> Die.utilA;
    Die.techA = s2[1];
    Die.utilA = Die.utilA / 100.;

    getIss(iss, line);
    iss >> s1 >> s2 >> Die.utilB;
    Die.techB = s2[1];
    Die.utilB = Die.utilB / 100.;
}

void cellParser(){
    istringstream iss;
    string line, s1, s2, s3;
    int t1, t2, t3;

    // eat newline
    getline(in_file, line);

    // build cell array
    getIss(iss, line);
    iss >> s1 >> t1;
    cellArray.emplace_back(new cell());
    while (t1--)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> s3;
        s3 = s3.substr(2);
        cellArray.emplace_back(new cell(stoi(s3)));
    }
}

void netParser(){
    istringstream iss;
    string line, s1, s2, s3;
    int t1, t2, t3;

    getline(in_file, line);

    // Build netArray
    getIss(iss, line);
    iss >> s1 >> t1;
    netArray.emplace_back(new net());
    for (int net_id = 1; net_id <= t1; net_id++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> t2;

        net *n = new net(t2);
        while (t2--)
        {
            getIss(iss, line);
            iss >> s1 >> s2;
            int cell_id = stoi(s2.substr(1));
            n->cells.insert(cell_id);
            // Also build cellArray
            cellArray[cell_id]->nets.insert(net_id);
        }
        netArray.emplace_back(n);
    }
}
void parser(string testcasePath)
{
    in_file.open(testcasePath);
    // ifstream in_file(testcasePath);

    if (in_file.fail())
    {
        cout << "Fail opening file: " << testcasePath << endl;
        exit(1);
    }

    libcellParser();
    DieInfoParser();
    cellParser();
    netParser();
 
    // check();

    in_file.close();
}
int main(int argc, char *argv[])
{
    auto [testcasePath, outputPath] = eatArg(argc, argv);
    parser(testcasePath);

    // for (auto item : TechA)
    // {
    //     cout << item.first << " " << item.second << endl;
    // }
    // for (auto item : TechB)
    // {
    //     cout << item.first << " " << item.second << endl;
    // }
}
