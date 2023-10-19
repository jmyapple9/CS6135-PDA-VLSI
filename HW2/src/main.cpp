#include "main.hpp"

// Utilities flag
bool TIME = false;

// Global data structures
ifstream in_file;
vector<pair<int, int>> techA, techB; // pair<w,t>
vector<cell *> cellArray;
vector<net *> netArray;
die Die;

// GLobal statistics
int NumTechs;

// Mapping table are used to prevent index mismapping of: Libcell, Cell, and Net
// R is the real id in name(eg. MC3, C8, N5)
// V is the virtual id = the position of node in array(techA, techB, cellArray, netArray)
unordered_map<int, int> lib_R_V, lib_V_R, cell_R_V, cell_V_R, net_R_V, net_V_R;

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
    for (int i = 1; i < techA.size(); i++)
    {
        cout << "MC" << lib_V_R[i] << ": " << techA[i].first << ", " << techA[i].second << endl;
    }
    if (NumTechs > 1)
    {
        cout << "LibCells B:" << endl;
        for (int i = 1; i < techB.size(); i++)
        {
            cout << "MC" << lib_V_R[i] << ": " << techB[i].first << ", " << techB[i].second << endl;
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
    int t1, t2, t3, LibRealId, LibVirId, libcellCount;

    // NumTechs
    getIss(iss, line);
    iss >> s1 >> NumTechs;

    // Tech TA's LibCell
    getIss(iss, line);
    iss >> s1 >> s2 >> libcellCount;

    techA.emplace_back(make_pair(-1, -1)); // put dummy node at index 0
    for (int LibVirId = 1; LibVirId <= libcellCount; LibVirId++)
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

        techB.emplace_back(make_pair(-1, -1)); // put dummy node at index 0
        for (int LibVirId = 1; LibVirId <= libcellCount; LibVirId++)
        {
            getIss(iss, line);
            iss >> s1 >> s2 >> t1 >> t2;
            techB.emplace_back(make_pair(t1, t2));
        }
    }
    else{
        techB = techA;
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
    Die.utilA /= 100;

    getIss(iss, line);
    iss >> s1 >> s2 >> Die.utilB;
    Die.techB = s2[1];
    Die.utilB /= 100.;

    Die.size = (long long) (Die.w * Die.h);
    // Die.Aarea = Die.Barea = Die.size;
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
            n->cells.push_back(cellVirId);
            // Also build cellArray
            cellArray[cellVirId]->nets.push_back(netVirId);
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
        printf("Parsing Time = %f\n", cpu_time_used);
}

void output(string outputPath)
{
    // !!! MUST REMEMBER !!!
    // !!! Use cell_V_R map virtual cell id to real cell id !!!
}

bool sortByAbs(const pair<int, int> &a, const pair<int, int> &b)
{
    // sort abs in decreasing
    return abs(a.first) > abs(b.first);
}

bool tryPutOn(pair<int, int> cellShapeInLib, char onDie)
{
    double area = cellShapeInLib.first * cellShapeInLib.second;

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
    // sort array in decreasing: D[d1, d2, d3... dn], where di be difference between cell area on chip A and B
    // from the cell having largest difference, put it into the chip has smaller difference
    vector<pair<int, int>> Diff;
    int cellcount = cellArray.size() - 1;
    for (int cellVirId = 1; cellVirId <= cellcount; cellVirId++)
    {
        int lib = cellArray[cellVirId]->lib;
        int areaA = techA[lib].first * techA[lib].second;
        int areaB = techB[lib].first * techB[lib].second;
        Diff.emplace_back(make_pair(areaB - areaA, cellVirId)); // Warning: areaA may > areaB, lead to negative
    }
    sort(Diff.begin(), Diff.end(), sortByAbs);
    int cnt = 0;
    for (auto areaCell : Diff)
    {
        int cellVirId = areaCell.second;
        int cellLibId = cellArray[cellVirId]->lib;
        if (areaCell.first > 0)
        { // cell size is bigger if made on B
            if (tryPutOn(techA[cellLibId], 'A'))
            { // first try put on A to minimize total area
                cellArray[cellVirId]->part = 'A';
            }
            else if (tryPutOn(techB[cellLibId], 'B'))
            { // if can't, try put on B
                cellArray[cellVirId]->part = 'B';
            }
            else
            {
                cout << "Die.utilA: " << Die.Aarea / Die.size << endl;
                double area = techA[cellLibId].first * techA[cellLibId].second;
                cout << "after put on one more cell: " << (Die.Aarea + area) / Die.size << endl;

                cout << "Die.utilB: " << Die.Barea / Die.size << endl;
                cout << "after put on one more cell: " << (Die.Barea + area) / Die.size << endl;

                cout << "lefted cell: " << Diff.size()-cnt << endl;
                cout << "Invalid initial partition(1)!" << endl;
                exit(1);
            }
        }
        else
        {
            if (tryPutOn(techB[cellLibId], 'B'))
            {
                cellArray[cellVirId]->part = 'B';
            }
            else if (tryPutOn(techA[cellLibId], 'A'))
            {
                cellArray[cellVirId]->part = 'A';
            }
            else
            {
                cout << "Invalid initial partition(2)!" << endl;
                exit(1);
            }
        }
        cnt++;
    }
    std::ofstream ofs;
    ofs.open("output.txt");
    if (!ofs.is_open())
    {
        cout << "Failed to open file.\n";
        exit;
    }
    for (int i = 1; i < cellArray.size(); i++)
    {
        ofs << "cell lib " << cellArray[i]->lib << ": " << cellArray[i]->part << endl;
    }
    ofs.close();
}

int main(int argc, char *argv[])
{
    auto [testcasePath, outputPath] = eatArg(argc, argv);
    parser(testcasePath);

    init_partition();

    output(outputPath);
}
