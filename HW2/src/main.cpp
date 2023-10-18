#include "main.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <time.h>

using namespace std;

// Utilities flag
bool TIME = true;

// Global data structures
ifstream in_file;
vector<pair<int, int>> TechA; // index is LibCell number-1 (ex. MC1 will be store at TechA[0])
vector<pair<int, int>> TechB; // pair<w,t>
vector<cell *> cellArray;
vector<net *> netArray;
die Die;

// GLobal statistics
int NumTechs;
int NumLibCells;

// Mapping table are used to prevent index mismapping of: Libcell, Cell, and Net
// R is the real id in name(eg. MC3, C8, N5)
// V is the virtual id = the position of node in array(TechA, TechB, cellArray, netArray)
unordered_map<int, int> lib_R_V;
unordered_map<int, int> lib_V_R;
unordered_map<int, int> cell_R_V;
unordered_map<int, int> cell_V_R;
unordered_map<int, int> net_R_V;
unordered_map<int, int> net_V_R;

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

void check()
{
    cout << "checking LibCells:" << endl;
    cout << "LibCells A:" << endl;
    for (int i = 1; i < TechA.size(); i++)
    {
        cout << "MC" << lib_V_R[i] << ": " << TechA[i].first << ", " << TechA[i].second << endl;
    }
    if (NumTechs > 1)
    {
        cout << "LibCells B:" << endl;
        for (int i = 1; i < TechB.size(); i++)
        {
            cout << "MC" << lib_V_R[i] << ": " << TechB[i].first << ", " << TechB[i].second << endl;
        }
    }

    cout << "checking net Array:" << endl;
    for (int i = 1; i < netArray.size(); i++)
    {
        cout << "In net N" << net_V_R[i] << " having " << netArray[i]->numCells << " cells:" << endl;
        for (int c : netArray[i]->cells)
        {
            cout << cell_V_R[c] << " ";
        }
        cout << endl;
    }
    cout << "\nchecking cell Array:" << endl;
    for (int i = 1; i < cellArray.size(); i++)
    {
        cout << "In cell C" << cell_V_R[i] << " having " << cellArray[i]->nets.size() << " nets:" << endl;
        for (int n : cellArray[i]->nets)
        {
            cout << net_V_R[n] << " ";
        }
        cout << endl;
    }
}

void libcellParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    int t1, t2, t3, LibRealId, LibVirId;

    // NumTechs
    getIss(iss, line);
    iss >> s1 >> NumTechs;

    // Tech TA's LibCell
    getIss(iss, line);
    iss >> s1 >> s2 >> NumLibCells;

    TechA.emplace_back(make_pair(-1, -1)); // put dummy node at index 0
    for (int LibVirId = 1; LibVirId <= NumLibCells; LibVirId++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> t1 >> t2;
        LibRealId = stoi(s2.substr(2));
        TechA.emplace_back(make_pair(t1, t2));
        lib_R_V[LibRealId] = LibVirId;
        lib_V_R[LibVirId] = LibRealId;
    }

    // Tech TB's LibCell, if exist
    if (NumTechs == 2)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> NumLibCells;

        TechB.emplace_back(make_pair(-1, -1)); // put dummy node at index 0
        for (int LibVirId = 1; LibVirId <= NumLibCells; LibVirId++)
        {
            getIss(iss, line);
            iss >> s1 >> s2 >> t1 >> t2;
            TechB.emplace_back(make_pair(t1, t2));
        }
    }
}

void DieInfoParser()
{
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

void cellParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    int cellCount, LibRealId, LibVirId, cellRealId;

    // eat newline
    getline(in_file, line);

    // build cell array
    getIss(iss, line);
    iss >> s1 >> cellCount;
    cellArray.emplace_back(new cell());
    for (int cellVirId = 1; cellVirId <= cellCount; cellVirId++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> s3;
        LibRealId = stoi(s3.substr(2));
        LibVirId = lib_R_V[LibRealId];
        cellArray.emplace_back(new cell(LibVirId));

        cellRealId = stoi(s2.substr(1));
        cell_R_V[cellRealId] = cellVirId;
        cell_V_R[cellVirId] = cellRealId;
    }
}

void netParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    int netCount, cellCount, netRealId, netVirId;

    getline(in_file, line);

    // Build netArray
    getIss(iss, line);
    iss >> s1 >> netCount;
    netArray.emplace_back(new net());
    for (int netVirId = 1; netVirId <= netCount; netVirId++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> cellCount;
        netRealId = stoi(s2.substr(1));

        net_R_V[netRealId] = netVirId;
        net_V_R[netVirId] = netRealId;

        net *n = new net(cellCount);
        while (cellCount--)
        {
            getIss(iss, line);
            iss >> s1 >> s2;
            int cellRealId = stoi(s2.substr(1));
            int cellVirId = cell_R_V[cellRealId];
            n->cells.insert(cellVirId);
            // Also build cellArray
            cellArray[cellVirId]->nets.insert(netVirId);
        }
        netArray.emplace_back(n);
    }
}
void parser(string testcasePath)
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();

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

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    if (TIME)
        printf("Time = %f\n", cpu_time_used);
}

void output(string outputPath)
{
    // !!! MUST REMEMBER !!!
    // !!! Use cell_V_R map virtual cell id to real cell id !!!
}
int main(int argc, char *argv[])
{
    auto [testcasePath, outputPath] = eatArg(argc, argv);
    parser(testcasePath);

    output(outputPath);
}
