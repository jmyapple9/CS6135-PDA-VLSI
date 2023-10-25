#include "main.hpp"

// Utilities flag
bool TIME = false;
bool DEBUGMODE = true;
// Global data structures
ifstream in_file;
BucketList *bListA, *bListB;
vector<pair<int, int>> techA, techB; // pair<w,t>
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
        cout << "MC" << lib_V_R[i] << ": " << techA[i].first << ", " << techA[i].second << endl;

    if (NumTechs > 1){
        cout << "LibCells B:" << endl;
        for (int i = 0; i < techB.size(); i++)
            cout << "MC" << lib_V_R[i] << ": " << techB[i].first << ", " << techB[i].second << endl;
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
    int t1, t2, LibRealId, libcellCount;

    // NumTechs
    getIss(iss, line);
    iss >> s1 >> NumTechs;

    // Tech TA's LibCell
    getIss(iss, line);
    iss >> s1 >> s2 >> libcellCount;
    for (int LibVirId = 0; LibVirId < libcellCount; LibVirId++){
        getIss(iss, line);
        iss >> s1 >> s2 >> t1 >> t2;
        LibRealId = stoi(s2.substr(2));
        techA.emplace_back(make_pair(t1, t2));
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
            techB.emplace_back(make_pair(t1, t2));
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
    // cout << "utilA: " << utilA << endl;
    // cout << "utilB: " << utilB << endl;
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

bool tryPutOn(int cellLibId, bool onDie, int swap)
{   
    // cout << "try put on " << (onDie?"A":"B")<< endl;
    pair<int, int> cellShapeInLib = (onDie==true) ? techA[cellLibId] : techB[cellLibId];
    long long area = static_cast<long long>(cellShapeInLib.first * cellShapeInLib.second);
    // cout << "area: " << area << ", " << "availA: " << die.availA << ", " << "availB: " << die.availB << endl;
    // cout << "area: " << area << ", " << "availA: " << die.availA - die.Aarea << ", " << "availB: " << die.availB - die.Barea << endl;

    if (onDie == true){
        if ((die.Aarea + area) < die.availA){
            die.Aarea += area;
            if(swap) die.Barea -= area;
            // cout << "A is avail !!!" << endl;
            return true;
        }
        else{
            // cout << "A not avail !!!" << endl;
            return false;
        }
    }
    else if (onDie == false){
        if ((die.Barea + area) < die.availB){
            die.Barea += area;
            if(swap) die.Aarea -= area;
            // cout << "B is avail !!!" << endl;
            return true;
        }
        else{
            // cout << "B not avail !!!" << endl;
            return false;
        }
    }
    else{
        cout << "Unknown Die: " << onDie << endl;
        return false;
    }
}

void init_partition()
{
    clock_t start = clock();
    vector<pair<int, int>> Diff;

    for (int cellVirId = 0; cellVirId < cellArray.size(); cellVirId++){
        // int lib = cellArray[cellVirId]->lib;
        int cellLibId = cellArray[cellVirId]->lib;
        int sizeOnA = techA[cellLibId].first * techA[cellLibId].second;
        int sizeOnB = techB[cellLibId].first * techB[cellLibId].second;

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
    bListA = new BucketList(pmax);
    bListB = new BucketList(pmax);

    if (TIME)
        printf("Initial partition Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
}

void init_distribution()
{
    cutsize = 0;
    for (auto n : netArray){
        int Ai = 0, Bi = 0;
        for (auto c : n->cells){
            (c->part == true) ? Ai++ : Bi++;
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

void init_cellGain()
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
            bListA->insert(gain, c);
        else
            bListB->insert(gain, c);
        // cout << "Done insertion!!" <<endl;
        if(0) cout << "Cell " << c->crid <<": " << c->gain << endl;
    }
    // cout << "Leaving init_cellGain..." << endl;

}

/* 
return: the Cell with larger Cell gain among bListA, bListB.
If this Cell is valid to move(under area constraint of dieA, dieB)
erase it from bucket list, and put it into free Cell list(locked Cell)
 */
Cell* maxGainCell(){
    // cout << "maxGainCell()" << endl;
    // bListA->show('A');
    // bListB->show('B');
    // listCellIter it;
    int validMaxGainA = bListA->maxGain;
    int validMaxGainB = bListB->maxGain;
    while(bListA->size() > 0 or bListB->size() > 0){
        // cout << "validMaxGainA: " << validMaxGainA << ", " << "validMaxGainB: " << validMaxGainB << endl;
        if(validMaxGainA >= validMaxGainB){ // put Cell from A to B
            if(validMaxGainA == -1) break;
            listCell& lcA = bListA->getMaxGainList(validMaxGainA--);
            if(!lcA.empty()){
                for(listCellIter it = lcA.begin(); it != lcA.end(); ){
                    if(tryPutOn((*it)->lib, false, 1)){
                        listCellIter rm = it;
                        Cell* cellNow = *it;
                        it++; // !!! Don't put it back to for loop !!!
                        bListA->erase(lcA, rm);
                        return cellNow;
                    }
                    else {
                        it++; // !!! Don't put it back to for loop !!!
                        continue;
                    }
                }
                // validMaxGainA--;
            }
            else{
                // validMaxGainA--;
                continue;
            }
        }else{ // put Cell from B to A
            if(validMaxGainB == -1) continue; // different from validMaxGainA: possibility: A=3, B=-1
            listCell& lcB = bListB->getMaxGainList(validMaxGainB--);
            if(!lcB.empty()){
                for(listCellIter it = lcB.begin(); it != lcB.end(); ){
                    if(tryPutOn((*it)->lib, true, 1)){
                        listCellIter rm = it;
                        Cell* cellNow = *it;
                        it++; // !!! Don't put it back to for loop !!!
                        bListB->erase(lcB, rm);
                        return cellNow;
                    }
                    else {
                        it++; // !!! Don't put it back to for loop !!!
                        continue;
                    }
                }
                // validMaxGainB--;
            }
            else{
                // validMaxGainB--;
                continue;
            }
        }
    }
    // exit(1);
    return nullptr;
    // for (bListIterator glA_It = bListA->gainList.begin(), glB_It = bListB->gainList.begin(); 
    // glA_It != bListA->gainList.end() && glB_It != bListB->gainList.end();){
    // }
}

/* 
Move the base Cell and update neighbor's gains
 */
void updateGain(Cell* baseCell){
    bool originPart = baseCell->part;
    baseCell->part = !baseCell->part;
    if(0) cout << "Turn cell " << baseCell->crid << " to " << (baseCell->part?"A":"B")<<endl;
    for (auto n : baseCell->nets){
        baseCell->lock = true;
        int F, T;
        if(originPart==true)
            F = n->distr.first, T = n->distr.second;
        else
            F = n->distr.second, T = n->distr.first;
        
        // int cnt = 0;
        // Cell* leftedOne;
        /* before move */
        if(T==0) {
            for(auto c: n->cells){
                if(!c->lock) c->gain++;
            }
            // baseCell gain: original -1, now +1, should be ++ one more times (should be meaningless: locked cell will not move in this pass)
            // baseCell->gain += 1;
        }
        else if(T==1){
            for(auto c: n->cells){
                if(originPart != c->part and !c->lock){
                    // cnt++, leftedOne = c;
                    c->gain--;
                }
            }
            // if(cnt==1) leftedOne->gain--;
        }
        /* Move */
        F--, T++;
        // baseCell->part = !baseCell->part;
        // cnt = 0;
        /* after move */
        if(F==0){
            for(auto c: n->cells){
                if(!c->lock) c->gain--; 
            }
            // baseCell gain: original +1, now -1, should be -- one more times (should be meaningless: locked cell will not move in this pass)
            // baseCell->gain -= 1;
        }
        else if(F==1){
            for(auto c: n->cells){
                if(originPart == c->part and !c->lock){
                    c->gain++;
                    // cnt++, leftedOne = c;
                }
                // if(cnt == 1) leftedOne->gain++;
            }
        }

    }
    
}

void pass(){
    // cout << "pass()" << endl;
    // Gk = 0;

    Cell* baseCell;
    init_distribution();
    init_cellGain();
    while((baseCell = maxGainCell())){
        if(0) cout << "basecell: " << baseCell->crid << ", gain=" << baseCell->gain << endl;

        // cout << "Enter pass's while loop" << endl;
        // Gk += baseCell->gain;
        steps.emplace_back(make_pair(baseCell->gain, baseCell->cvid));
        // cout << "update gain" << endl;
        updateGain(baseCell);
        // cout << "Done update gain" << endl;
    }
    // cout << "Leaving pass..." << endl;
    // cout << endl;
    // cout << "End pass()" << endl;
    

}

pair<int,int> getBestMove(){
    int Gk = 0;
    int k = steps.size();
    // int prefix[k];
    // fill(prefix.begin(), prefix.end(), 0);
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
    // cout << "Gain seq: ";
    // for(int i = 0; i<k; i++) cout << steps[i].first << " ";
    // cout << "\nreturn best move index " << bestSeqIdx << endl;
    // cout << "Gk: " << Gk << endl;
    return {bestSeqIdx, Gk};
}

void FM(){
    // cout << "FM()" << endl;
    while(1){
        pass();
        auto [moveTo, Gk] = getBestMove();
        // if(1) cout << "moveTo: " << moveTo << ", " << "Gk: " << Gk << endl;
        if(moveTo == -1 or Gk <= 0){
            if(DEBUGMODE) cout << "Finishing FM..." << endl;
            for(auto p: steps){
                Cell* c = cellArray[p.second];
                c->part = !c->part;
            }
            // init_distribution();
            break;
        }

        for(int i = steps.size()-1; i>moveTo; i--){
            Cell* c = cellArray[steps[i].second];
            c->part = !c->part;
            // cout << 
        }
        
        steps = {};
        bListA->clear();
        bListB->clear();

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
    init_partition();
    // init_distribution();

    FM();

    output(outputPath);

    if (TIME){
        cout << "-------------------------------" << endl;
        printf("Total exe Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
    }
}
