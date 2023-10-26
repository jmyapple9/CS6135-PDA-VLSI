#include "main.hpp"

// Utilities flag
bool TIME = false;
bool DEBUGMODE = true;
// Global data structures
ifstream in_file;
// BucketList *bListA, *bListB;
vector<long long> techA, techB; // pair<w,t>
vector<Cell *> cellArray;
vector<Net *> netArray;
vector<pair<int,int>> steps; // first:Cell gain, second: Cell vir id
// vector<Cell *> freeCellList; // after the pass, just put item back to bListA, bListB. Accroding to their new gains.
Die die;

// GLobal statistics
int NumTechs;
int pmax;
int cutsize;
// int Gk;
// Mapping table are used to prevent index mismapping of: Libcell, Cell, and Net
// R is the real id in name(eg. MC3, C8, N5)
// V is the virtual id = the position of node in array(techA, techB, cellArray, netArray)
unordered_map<int, int> lib_R_V, lib_V_R, cell_R_V, cell_V_R, net_R_V, net_V_R; // some of them can be delete if code is bug free

pair<string, string> eatArg(int argc, char *argv[]){
    if (argc != 3){
        cout << "Input format should be the following." << endl;
        cout << "eg.  ./hw2 ../testcase/public1.txt ../output/public1.out" << endl;
        cout << "exiting..." << endl;
        exit(1);
    }
    return {argv[1], argv[2]};
}

void getIss(istringstream &iss, string line){
    iss.clear();
    iss.str("");
    getline(in_file, line);
    iss.str(line);
}

void check(){
    cout << "checking LibCells:" << endl;
    cout << "LibCells A:" << endl;
    for (int i = 0; i < techA.size(); i++)
        cout << "MC" << lib_V_R[i] << ": " << techA[i]<< endl;

    if (NumTechs > 1){
        cout << "LibCells B:" << endl;
        for (int i = 0; i < techB.size(); i++)
            cout << "MC" << lib_V_R[i] << ": " << techB[i] << endl;
    }

    cout << "checking Net Array:" << endl;
    for (int i = 0; i < netArray.size(); i++){
        cout << "In Net N" << net_V_R[i] << " having " << netArray[i]->numCells << " cells:" << endl;
        for (auto c : netArray[i]->cells)
            cout << c->crid << " ";

        cout << endl;
    }
    cout << "\nchecking Cell Array:" << endl;
    for (int i = 0; i < cellArray.size(); i++){
        cout << "In Cell C" << cell_V_R[i] << " having " << cellArray[i]->nets.size() << " nets:" << endl;
        for (auto n : cellArray[i]->nets)
            cout << n->nrid << " ";
        
        cout << endl;
    }
}

void libParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    int LibRealId, libcellCount;
    long long t1, t2;
    // NumTechs
    getIss(iss, line);
    iss >> s1 >> NumTechs;

    // Tech TA's LibCell
    getIss(iss, line);
    iss >> s1 >> s2 >> libcellCount;
    techA.reserve(libcellCount);
    techB.reserve(libcellCount);

    for (int LibVirId = 0; LibVirId < libcellCount; LibVirId++){
        getIss(iss, line);
        iss >> s1 >> s2 >> t1 >> t2;
        LibRealId = stoi(s2.substr(2));
        techA.emplace_back(t1*t2);
        lib_R_V[LibRealId] = LibVirId;
        lib_V_R[LibVirId] = LibRealId;
    }

    // Tech TB's LibCell, if exist
    if (NumTechs > 1){
        getIss(iss, line);
        iss >> s1 >> s2 >> libcellCount;
        for (int LibVirId = 0; LibVirId < libcellCount; LibVirId++){
            getIss(iss, line);
            iss >> s1 >> s2 >> t1 >> t2;
            techB.emplace_back(t1*t2);
        }
    }
    else
        techB = techA;
    
}

void DieInfoParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    double t1;
    double utilA, utilB;
    // eat newline
    getline(in_file, line);

    // die info
    getIss(iss, line);
    iss >> s1 >> die.w >> die.h;

    getIss(iss, line);
    iss >> s1 >> s2 >> t1;
    die.techA = s2[1];
    utilA = t1 / 100.;

    getIss(iss, line);
    iss >> s1 >> s2 >> t1;
    die.techB = s2[1];
    utilB = t1 / 100.;
    die.size = (die.w * die.h);
    die.availA = die.size * utilA;
    die.availB = die.size * utilB;

}

void cellParser()
{
    istringstream iss;
    string line, s1, s2, s3;
    int cellCount, LibRealId, LibVirId, cellRealId;

    // eat newline
    getline(in_file, line);

    // build Cell array
    getIss(iss, line);
    iss >> s1 >> cellCount;
    cellArray.reserve(cellCount);
    steps.reserve(cellCount);

    for (int cellVirId = 0; cellVirId < cellCount; cellVirId++){
        getIss(iss, line);
        iss >> s1 >> s2 >> s3;
        LibRealId = stoi(s3.substr(2));
        LibVirId = lib_R_V[LibRealId];
        cellArray.emplace_back(new Cell(LibVirId, cellVirId));

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
    netArray.reserve(netCount);
    for (int netVirId = 0; netVirId < netCount; netVirId++){
        getIss(iss, line);
        iss >> s1 >> s2 >> cellCount;
        netRealId = stoi(s2.substr(1));

        Net *n = new Net(cellCount);
        netArray.emplace_back(n);
        net_R_V[netRealId] = netVirId;
        net_V_R[netVirId] = netRealId;
        netArray[netVirId]->nrid = netRealId; // only for debug

        while (cellCount--){
            getIss(iss, line);
            iss >> s1 >> s2;
            int cellRealId = stoi(s2.substr(1));
            int cellVirId = cell_R_V[cellRealId];
            n->cells.push_back(cellArray[cellVirId]);
            // Also build cellArray
            list<Net*>& nets = cellArray[cellVirId]->nets;
            nets.push_back(n);
            int numP = nets.size();
            pmax = max(pmax, numP);
        }
    }
}

void parser(string testcasePath){
    clock_t start;
    start = clock();

    in_file.open(testcasePath);

    if (in_file.fail()){
        cout << "Fail opening file: " << testcasePath << endl;
        exit(1);
    }

    libParser();
    DieInfoParser();
    cellParser();
    netParser();

    // check();

    in_file.close();

    if (TIME)
        printf("Parsing Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
}

void output(string outputPath){
    // !!! MUST REMEMBER !!!
    // !!! Use cell_V_R map virtual Cell id to real Cell id !!!
    ofstream outputfile;
    outputfile.open(outputPath);

    vector<Cell*> setA, setB;
    for(auto c:cellArray){
        if(c->part==true) setA.emplace_back(c);
        else setB.emplace_back(c);
    }

    outputfile << "CutSize " << cutsize << '\n';
    outputfile << "DieA " << setA.size()<<'\n';
    for(auto c: setA)
        outputfile << "C" << c->crid << '\n';

    outputfile << "DieB " << setB.size()<<'\n';
    for(auto c: setB)
        outputfile << "C" << c->crid << '\n';
    
    outputfile.flush();
    outputfile.close();
}

bool tryPutOn(int cellLibId, bool toDie, int move)
{
    if (toDie == true){
        long long T_area = techA[cellLibId];
        long long F_area = techB[cellLibId];
        if ((die.Aarea + T_area) < die.availA){
            die.Aarea += T_area;
            if(move) die.Barea -= F_area;
            return true;
        }
        else{
            return false;
        }
    }
    else{
        long long T_area = techB[cellLibId];
        long long F_area = techA[cellLibId];
        if ((die.Barea + T_area) < die.availB){
            die.Barea += T_area;
            if(move) die.Aarea -= F_area;
            return true;
        }
        else{
            return false;
        }
    }
}

int init_partition()
{
    clock_t start = clock();
    vector<pair<int, int>> Diff;

    for (int cellVirId = 0; cellVirId < cellArray.size(); cellVirId++){
        // int lib = cellArray[cellVirId]->lib;
        int cellLibId = cellArray[cellVirId]->lib;
        int sizeOnA = techA[cellLibId];
        int sizeOnB = techB[cellLibId];

        if (sizeOnA <= sizeOnB){ // can't delete this line  !!!
            if (tryPutOn(cellLibId, true, 0))
                cellArray[cellVirId]->part = true;
            else if (tryPutOn(cellLibId, false, 0))
                cellArray[cellVirId]->part = false;
            else{
                cout << "Invalid initial partition!" << endl;
                cellArray[cellVirId]->part = true;
                // exit(1);
            }
        }
        else{ // can't delete this line  !!!
            if (tryPutOn(cellLibId, false, 0))
                cellArray[cellVirId]->part = false;
            else if (tryPutOn(cellLibId, true, 0))
                cellArray[cellVirId]->part = true;
            else{
                cout << "Invalid initial partition!" << endl;
                cellArray[cellVirId]->part = true;
                // exit(1);
            }
        }
    }
    // ofstream myfile;
    // myfile.open ("output.txt");
    // int i = 0;
    // for(auto c:cellArray)
    //     cout << "Cell " << ++i << " in " << c->part <<endl;
    // myfile.close();
    // bListA = new BucketList(pmax);
    // bListB = new BucketList(pmax);

    if (TIME)
        printf("Initial partition Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
    return pmax;
}

void init_distribution()
{
    // cout << "init_distribution()" << endl;
    cutsize = 0;
    for (auto n : netArray){
        int Ai = 0, Bi = 0;
        for (auto c : n->cells){
            (c->part) ? Ai++ : Bi++;
        }
        n->distr.first = Ai;
        n->distr.second = Bi;

        if(Ai != n->cells.size() and Bi != n->cells.size())
            cutsize++;
    }
    cout << "cutsize: " << cutsize << endl;
    if(0) {
        for(int i = 0; i < cellArray.size(); i++){
            cout << "Cell " << i+1 << " is on " << (cellArray[i]->part?"A":"B") << endl;
        }

        for(int i = 0; i < netArray.size(); i++){
            cout << "Net " << i+1 << " has "<<netArray[i]->cells.size() << " cells: "\
            << netArray[i]->distr.first << ", "<<netArray[i]->distr.second<<endl;
        }
    }
}

void init_cellGain(BucketList& bListA, BucketList& bListB)
{
    // cout << "init_cellGain()" << endl;
    for (auto c : cellArray){
        int gain = 0;
        for (auto n : c->nets){
            int F, T;
            // Net* n = netArray[netVirId];
            if (c->part == true)
                F = n->distr.first, T = n->distr.second;
            else
                F = n->distr.second, T = n->distr.first;

            if (F == 1)
                gain++;
            if (T == 0)
                gain--;
        }
        if (c->part == true)
            bListA.insert(gain, c);
        else
            bListB.insert(gain, c);
        if(0) cout << "Cell " << c->crid <<": " << c->gain << endl;
    }
}

/* 
return: the Cell with larger Cell gain among bListA, bListB.
If this Cell is valid to move(under area constraint of dieA, dieB)
erase it from bucket list, and put it into free Cell list(locked Cell)
 */
Cell* maxGainCell(BucketList& bListA, BucketList& bListB){
    int validMaxGainA = bListA.maxGain;
    int validMaxGainB = bListB.maxGain;
    while( !(validMaxGainA == -1 and validMaxGainB == -1) ){
        if(validMaxGainA >= validMaxGainB){ // put Cell from A to B
            // cout << "validMaxGainA=" << validMaxGainA<<endl;
            if(validMaxGainA == -1) break;
            listCell& lcA = bListA.getMaxGainList(validMaxGainA--);
            if (!lcA.empty()) {
                for (auto it = lcA.begin(); it != lcA.end(); ) {
                    if (tryPutOn((*it)->lib, false, 1)) {
                        Cell* cellNow = *it;
                        it = bListA.erase(lcA, it);
                        return cellNow;
                    }
                    else {
                        it = bListA.erase(lcA, it);
                        // ++it;
                    }
                }
            }
            else{
                continue;
            }
        }else{ // put Cell from B to A
            // cout << "validMaxGainB=" << validMaxGainB<<endl;
            if(validMaxGainB == -1) continue; // different from validMaxGainA: possibility: A=3, B=-1
            listCell& lcB = bListB.getMaxGainList(validMaxGainB--);
            if (!lcB.empty()) {
                for (auto it = lcB.begin(); it != lcB.end(); ) {
                    if (tryPutOn((*it)->lib, true, 1)) {
                        Cell* cellNow = *it;
                        it = bListB.erase(lcB, it);
                        return cellNow;
                    }
                    else {
                        it = bListB.erase(lcB, it);
                        // ++it;
                    }
                }
            }
            else{
                continue;
            }
        }
    }
    return nullptr;
}

/* 
Move the base Cell and update neighbor's gains
 */
void updateGain(Cell* baseCell){
    bool originPart = baseCell->part;
    baseCell->part = !baseCell->part;
    baseCell->lock = true;

    for (auto n : baseCell->nets){
        int F, T;
        if(originPart==true)
            F = n->distr.first, T = n->distr.second;
        else
            F = n->distr.second, T = n->distr.first;
        
        /* before move */
        if(T==0) {
            for(auto c: n->cells){
                if(!c->lock) c->gain++;
            }
        }
        else if(T==1){
            for(auto c: n->cells){
                if(originPart != c->part and !c->lock){
                    c->gain--;
                }
            }
        }

        /* Move */
        F--, T++;

        /* after move */
        if(F==0){
            for(auto c: n->cells){
                if(!c->lock) c->gain--; 
            }
        }
        else if(F==1){
            for(auto c: n->cells){
                if(originPart == c->part and !c->lock){
                    c->gain++;
                }
            }
        }

        if(originPart==true)
            n->distr.first = F, n->distr.second = T;
        else
            n->distr.second = F, n->distr.first = T;

    }
    
}

void pass(BucketList& bListA, BucketList& bListB){
    Cell* baseCell;
    init_distribution();
    init_cellGain(bListA, bListB);
    // cout << "Enter pass while loop..." << endl;
    while((baseCell = maxGainCell(bListA, bListB))){
        // cout << "Find max!" <<endl;
        steps.emplace_back(make_pair(baseCell->gain, baseCell->cvid));
        updateGain(baseCell);
    }
}

pair<int,int> getBestMove(){
    int Gk = 0;
    int k = steps.size();

    vector<int> prefix(k, 10);
    prefix[0] = steps[0].first;
    for(int i = 1; i < k; i++){
        prefix[i] = prefix[i-1] + steps[i].first;
    }

    int bestSeqIdx=-1;
    Gk = prefix[0];
    for(int i = 0; i < k; i++){
        if(Gk <= prefix[i]) {
            bestSeqIdx = i;
            Gk = prefix[i];
        }
    }
    return {bestSeqIdx, Gk};
}

void FM(int pmax){
    // cout << "start FM()" << endl;
    while(1){
        BucketList bListA = BucketList(pmax);
        BucketList bListB = BucketList(pmax);
        pass(bListA, bListB);
        // cout << "Reversing..." <<endl;
        auto [moveTo, Gk] = getBestMove();
        if(moveTo == -1 or Gk <= 0){
            if(DEBUGMODE) cout << "Finishing FM..." << endl;
            for(auto p: steps){
                Cell* c = cellArray[p.second];
                c->part = !c->part;
            }
            break;
        }

        for(int i = steps.size()-1; i>moveTo; i--){
            Cell* c = cellArray[steps[i].second];
            c->part = !c->part;
            int cellLibId = c->lib;
            // long long T_area = (c->part) ? techA[cellLibId] : techB[cellLibId];
            // long long F_area = (c->part) ? techB[cellLibId] : techA[cellLibId];
            if(c->part){
                die.Aarea += techA[cellLibId];
                die.Barea -= techB[cellLibId];
            }else{
                die.Barea += techB[cellLibId];
                die.Aarea -= techA[cellLibId];
            }

        }
        steps = {};
        // bListA.clear();
        // bListB.clear();

        for(Cell* c: cellArray) {
            c->lock = false; // unlock all cells
        }
        
    }
}

int main(int argc, char *argv[]){
    clock_t start = clock();
    auto [testcasePath, outputPath] = eatArg(argc, argv);

    parser(testcasePath);
    // check();
    int pmax = init_partition();
    // init_distribution();

    FM(pmax);

    output(outputPath);

    if (TIME){
        cout << "-------------------------------" << endl;
        printf("Total exe Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
    }
}
