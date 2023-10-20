#include "main.hpp"

// Utilities flag
bool TIME = false;

// Global data structures
ifstream in_file;
bucketList *bListA, *bListB;
vector<pair<int, int>> techA, techB; // pair<w,t>
vector<cell *> cellArray;
vector<net *> netArray;
die Die;

// GLobal statistics
int NumTechs;
int pmax = 0;
// Mapping table are used to prevent index mismapping of: Libcell, Cell, and Net
// R is the real id in name(eg. MC3, C8, N5)
// V is the virtual id = the position of node in array(techA, techB, cellArray, netArray)
unordered_map<int, int> lib_R_V, lib_V_R, cell_R_V, cell_V_R, net_R_V, net_V_R; // some of them can be delete if code is bug free

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
    for (int i = 0; i < techA.size(); i++)
    {
        cout << "MC" << lib_V_R[i] << ": " << techA[i].first << ", " << techA[i].second << endl;
    }
    if (NumTechs > 1)
    {
        cout << "LibCells B:" << endl;
        for (int i = 0; i < techB.size(); i++)
        {
            cout << "MC" << lib_V_R[i] << ": " << techB[i].first << ", " << techB[i].second << endl;
        }
    }

    cout << "checking net Array:" << endl;
    for (int i = 0; i < netArray.size(); i++)
    {
        cout << "In net N" << net_V_R[i] << " having " << netArray[i]->numCells << " cells:" << endl;
        for (auto c : netArray[i]->cells)
        {
            cout << c->crid << " ";
        }
        cout << endl;
    }
    cout << "\nchecking cell Array:" << endl;
    for (int i = 0; i < cellArray.size(); i++)
    {
        cout << "In cell C" << cell_V_R[i] << " having " << cellArray[i]->nets.size() << " nets:" << endl;
        for (auto n : cellArray[i]->nets)
        {
            cout << n->nrid << " ";
        }
        cout << endl;
    }
}

void libcellParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    int t1, t2, t3, LibRealId, LibVirId, libcellCount;

    // NumTechs
    getIss(iss, line);
    iss >> s1 >> NumTechs;

    // Tech TA's LibCell
    getIss(iss, line);
    iss >> s1 >> s2 >> libcellCount;
    for (int LibVirId = 0; LibVirId < libcellCount; LibVirId++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> t1 >> t2;
        LibRealId = stoi(s2.substr(2));
        techA.emplace_back(make_pair(t1, t2));
        lib_R_V[LibRealId] = LibVirId;
        lib_V_R[LibVirId] = LibRealId;
    }

    // Tech TB's LibCell, if exist
    if (NumTechs > 1)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> libcellCount;
        for (int LibVirId = 0; LibVirId < libcellCount; LibVirId++)
        {
            getIss(iss, line);
            iss >> s1 >> s2 >> t1 >> t2;
            techB.emplace_back(make_pair(t1, t2));
        }
    }
    else
    {
        techB = techA;
    }
}

void DieInfoParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    double t1;

    // eat newline
    getline(in_file, line);

    // Die info
    getIss(iss, line);
    iss >> s1 >> Die.w >> Die.h;

    getIss(iss, line);
    iss >> s1 >> s2 >> t1;
    Die.techA = s2[1];
    Die.utilA = t1 / 100.;

    getIss(iss, line);
    iss >> s1 >> s2 >> t1;
    Die.techB = s2[1];
    Die.utilB = t1 / 100.;

    Die.size = (Die.w * Die.h);
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
    for (int cellVirId = 0; cellVirId < cellCount; cellVirId++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> s3;
        LibRealId = stoi(s3.substr(2));
        LibVirId = lib_R_V[LibRealId];
        cellArray.emplace_back(new cell(LibVirId));

        cellRealId = stoi(s2.substr(1));
        cell_R_V[cellRealId] = cellVirId;
        cell_V_R[cellVirId] = cellRealId;
        cellArray[cellVirId]->crid = cellRealId; // only for debug
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
    for (int netVirId = 0; netVirId < netCount; netVirId++)
    {
        getIss(iss, line);
        iss >> s1 >> s2 >> cellCount;
        netRealId = stoi(s2.substr(1));

        net *n = new net(cellCount);
        netArray.emplace_back(n);
        net_R_V[netRealId] = netVirId;
        net_V_R[netVirId] = netRealId;
        netArray[netVirId]->nrid = netRealId; // only for debug

        while (cellCount--)
        {
            getIss(iss, line);
            iss >> s1 >> s2;
            int cellRealId = stoi(s2.substr(1));
            int cellVirId = cell_R_V[cellRealId];
            n->cells.push_back(cellArray[cellVirId]);
            // Also build cellArray
            cellArray[cellVirId]->nets.push_back(n);
            int numP = cellArray[cellVirId]->nets.size();
            pmax = max(pmax, numP);
        }
    }
}

void parser(string testcasePath)
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    in_file.open(testcasePath);

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

    if (TIME)
        printf("Parsing Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
}

void output(string outputPath)
{
    // !!! MUST REMEMBER !!!
    // !!! Use cell_V_R map virtual cell id to real cell id !!!
}

bool tryPutOn(pair<int, int> cellShapeInLib, char onDie)
{
    cpp_dec_float_50 area = static_cast<cpp_dec_float_50>(cellShapeInLib.first * cellShapeInLib.second);
    // double area = cellShapeInLib.first * cellShapeInLib.second;
    if (onDie == 'A')
    {
        if ((Die.Aarea + area) / Die.size < Die.utilA)
        {
            Die.Aarea += area;
            return true;
        }
        else
            return false;
    }
    else if (onDie == 'B')
    {
        if ((Die.Barea + area) / Die.size < Die.utilB)
        {
            Die.Barea += area;
            return true;
        }
        else
            return false;
    }
    else
    {
        cout << "Unknown die: " << onDie << endl;
        return false;
    }
}

void init_partition()
{
    clock_t start = clock();
    vector<pair<int, int>> Diff;

    for (int cellVirId = 0; cellVirId < cellArray.size(); cellVirId++)
    {
        int lib = cellArray[cellVirId]->lib;
        int areaA = techA[lib].first * techA[lib].second;
        int areaB = techB[lib].first * techB[lib].second;
        int cellLibId = cellArray[cellVirId]->lib;

        if (areaA <= areaB)
        { // can't delete this line  !!!
            if (tryPutOn(techA[cellLibId], 'A'))
                cellArray[cellVirId]->part = 'A';
            else if (tryPutOn(techB[cellLibId], 'B'))
                cellArray[cellVirId]->part = 'B';
            else
            {
                cout << "Invalid initial partition!" << endl;
                cellArray[cellVirId]->part = 'A';
                // exit(1);
            }
        }
        else
        { // can't delete this line  !!!
            if (tryPutOn(techB[cellLibId], 'B'))
                cellArray[cellVirId]->part = 'B';
            else if (tryPutOn(techA[cellLibId], 'A'))
                cellArray[cellVirId]->part = 'A';
            else
            {
                cout << "Invalid initial partition!" << endl;
                cellArray[cellVirId]->part = 'A';
                // exit(1);
            }
        }
    }
    // ofstream myfile;
    // myfile.open ("output.txt");
    // int i = 0;
    // for(auto c:cellArray)
    //     cout << "cell " << ++i << " in " << c->part <<endl;
    // myfile.close();

    if (TIME)
        printf("Initial partition Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
}

void init_distribution()
{
    for (auto n : netArray)
    {
        int Ai = 0, Bi = 0;
        for (auto c : n->cells)
        {
            if (c->part == 'A')
                Ai++;
            else if (c->part == 'B')
                Bi++;
            else
                cout << "Error! cell isn't in neither part A or B!" << endl;
        }
        n->distr.first = Ai;
        n->distr.second = Bi;
    }

    // for(int i = 0; i < netArray.size(); i++){
    //     cout << "net " << i+1 << " has "<<netArray[i]->cells.size() << " cells: "\
    //     << netArray[i]->distr.first << ", "<<netArray[i]->distr.second<<endl;
    // }
}

void init_cellGain()
{
    for (auto c : cellArray)
    {
        int gain = 0;
        for (auto n : c->nets)
        {
            int F, T;
            // net* n = netArray[netVirId];
            if (c->part == 'A')
                F = n->distr.first, T = n->distr.second;
            else
                F = n->distr.second, T = n->distr.first;

            if (F == 1)
                gain++;
            if (T == 0)
                gain--;
        }
        if (c->part == 'A')
            bListA->insert(gain, c);
        else
            bListB->insert(gain, c);
    }
}

void init_bucketList()
{
    bListA = new bucketList(pmax);
    bListB = new bucketList(pmax);
    init_distribution();
    init_cellGain();
}

int main(int argc, char *argv[])
{
    clock_t start = clock();
    auto [testcasePath, outputPath] = eatArg(argc, argv);

    parser(testcasePath);
    // check();
    init_partition();

    init_bucketList();

    output(outputPath);

    if (TIME)
    {
        cout << "-------------------------------" << endl;
        printf("Total exe Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
    }
}
