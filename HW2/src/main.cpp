#include "main.hpp"

// Utilities flag
bool TIME = false;

// Global data structures
ifstream in_file;
bucketList *bListA, *bListB;
vector<pair<int, int>> techA, techB; // pair<w,t>
vector<cell *> cellArray;
vector<net *> netArray;
vector<int> steps; // cell vir id
// vector<cell *> freeCellList; // after the pass, just put item back to bListA, bListB. Accroding to their new gains.
die Die;

// GLobal statistics
int NumTechs;
int pmax;
int cutsize;
int Gk;
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

    cout << "checking net Array:" << endl;
    for (int i = 0; i < netArray.size(); i++){
        cout << "In net N" << net_V_R[i] << " having " << netArray[i]->numCells << " cells:" << endl;
        for (auto c : netArray[i]->cells)
            cout << c->crid << " ";

        cout << endl;
    }
    cout << "\nchecking cell Array:" << endl;
    for (int i = 0; i < cellArray.size(); i++){
        cout << "In cell C" << cell_V_R[i] << " having " << cellArray[i]->nets.size() << " nets:" << endl;
        for (auto n : cellArray[i]->nets)
            cout << n->nrid << " ";
        
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

    // Die info
    getIss(iss, line);
    iss >> s1 >> Die.w >> Die.h;

    getIss(iss, line);
    iss >> s1 >> s2 >> t1;
    Die.techA = s2[1];
    utilA = t1 / 100.;

    getIss(iss, line);
    iss >> s1 >> s2 >> t1;
    Die.techB = s2[1];
    utilB = t1 / 100.;

    Die.size = (Die.w * Die.h);
    Die.availA = Die.size * utilA;
    Die.availB = Die.size * utilB;
    // cout << "Die.size: " << Die.size <<endl;
    // cout << "DieA available: " << Die.availA <<endl;
    // cout << "DieB available: " << Die.availB <<endl;

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
    for (int cellVirId = 0; cellVirId < cellCount; cellVirId++){
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
    for (int netVirId = 0; netVirId < netCount; netVirId++){
        getIss(iss, line);
        iss >> s1 >> s2 >> cellCount;
        netRealId = stoi(s2.substr(1));

        net *n = new net(cellCount);
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
            list<net*>& nets = cellArray[cellVirId]->nets;
            nets.push_back(n);
            int numP = nets.size();
            pmax = max(pmax, numP);
        }
    }
}

void parser(string testcasePath){
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    in_file.open(testcasePath);

    if (in_file.fail()){
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

void output(string outputPath){
    // !!! MUST REMEMBER !!!
    // !!! Use cell_V_R map virtual cell id to real cell id !!!
    ofstream outputfile;
    outputfile.open(outputPath);

    vector<cell*> setA, setB;
    for(auto c:cellArray){
        if(c->part==true) setA.emplace_back(c);
        else setB.emplace_back(c);
    }

    outputfile << "CutSize " << cutsize << endl;
    outputfile << "DieA " << setA.size()<<endl;
    for(auto c: setA)
        outputfile << "C" << c->crid << endl;

    outputfile << "DieB " << setB.size()<<endl;
    for(auto c: setB)
        outputfile << "C" << c->crid << endl;

    outputfile.close();
}

bool tryPutOn(int cellLibId, bool onDie)
{   
    // cout << "try put on " << (onDie?"A":"B")<< endl;
    pair<int, int> cellShapeInLib = (onDie==true) ? techA[cellLibId] : techB[cellLibId];
    long long area = static_cast<long long>(cellShapeInLib.first * cellShapeInLib.second);
    // cout << "area: " << area << ", " << "availA: " << Die.availA << ", " << "availB: " << Die.availB << endl;
    // cout << "area: " << area << ", " << "availA: " << Die.availA - Die.Aarea << ", " << "availB: " << Die.availB - Die.Barea << endl;

    if (onDie == true){
        if ((Die.Aarea + area) < Die.availA){
            Die.Aarea += area;
            // cout << "A is avail !!!" << endl;
            return true;
        }
        else{
            // cout << "A not avail !!!" << endl;
            return false;
        }
    }
    else if (onDie == false){
        if ((Die.Barea + area) < Die.availB){
            Die.Barea += area;
            // cout << "B is avail !!!" << endl;
            return true;
        }
        else{
            // cout << "B not avail !!!" << endl;
            return false;
        }
    }
    else{
        cout << "Unknown die: " << onDie << endl;
        return false;
    }
}

void init_partition()
{
    clock_t start = clock();
    vector<pair<int, int>> Diff;

    for (int cellVirId = 0; cellVirId < cellArray.size(); cellVirId++){
        int lib = cellArray[cellVirId]->lib;
        int areaA = techA[lib].first * techA[lib].second;
        int areaB = techB[lib].first * techB[lib].second;
        int cellLibId = cellArray[cellVirId]->lib;

        if (areaA <= areaB){ // can't delete this line  !!!
            if (tryPutOn(cellLibId, true))
                cellArray[cellVirId]->part = true;
            else if (tryPutOn(cellLibId, false))
                cellArray[cellVirId]->part = false;
            else{
                cout << "Invalid initial partition!" << endl;
                cellArray[cellVirId]->part = true;
                // exit(1);
            }
        }
        else{ // can't delete this line  !!!
            if (tryPutOn(cellLibId, false))
                cellArray[cellVirId]->part = false;
            else if (tryPutOn(cellLibId, true))
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
    //     cout << "cell " << ++i << " in " << c->part <<endl;
    // myfile.close();
    bListA = new bucketList(pmax);
    bListB = new bucketList(pmax);

    if (TIME)
        printf("Initial partition Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
}

void init_distribution()
{
    cutsize = 0;
    for (auto n : netArray){
        int Ai = 0, Bi = 0;
        for (auto c : n->cells){
            if (c->part == true)
                Ai++;
            else if (c->part == false)
                Bi++;
            else
                cout << "Error! cell isn't in neither part A or B!" << endl;
        }
        n->distr.first = Ai;
        n->distr.second = Bi;

        if(Ai != n->cells.size() and Bi != n->cells.size())
            cutsize++;
    }
    cout << "cutsize: " << cutsize << endl;

    // for(int i = 0; i < netArray.size(); i++){
    //     cout << "net " << i+1 << " has "<<netArray[i]->cells.size() << " cells: "\
    //     << netArray[i]->distr.first << ", "<<netArray[i]->distr.second<<endl;
    // }
}

void init_cellGain()
{
    for (auto c : cellArray){
        int gain = 0;
        for (auto n : c->nets){
            int F, T;
            // net* n = netArray[netVirId];
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
        
        // cout << "cell " << c->crid <<": " << c->gain << endl;
    }
}

/* 
return: the cell with larger cell gain among bListA, bListB.
If this cell is valid to move(under area constraint of dieA, dieB)
erase it from bucket list, and put it into free cell list(locked cell)
 */
cell* maxGainCell(){
    // cout << "maxGainCell()" << endl;
    // bListA->show('A');
    // bListB->show('B');
    listCellIter it;
    int validMaxGainA = bListA->maxGain;
    int validMaxGainB = bListA->maxGain;
    while(bListA->size() > 0 or bListB->size() > 0){
        // cout << "validMaxGainA: " << validMaxGainA << ", " << "validMaxGainB: " << validMaxGainB << endl;
        if(validMaxGainA >= validMaxGainB){
            if(validMaxGainA == -1) break;
            listCell& lcA = bListA->getMaxGainList(validMaxGainA);
            // cout << "validMaxGainA: " << validMaxGainA << endl;
            if(!lcA.empty()){
                // cout << "lcA not empty" << endl;
                for(listCellIter lcAit = lcA.begin(); lcAit != lcA.end(); ){
                    if(tryPutOn((*lcAit)->lib, false)){
                        // cout << "Sucessfully put cell " << cellNow->crid << " on A" <<endl; 
                        listCellIter rm = lcAit;
                        cell* cellNow = *lcAit;
                        lcAit++; // !!! Don't put it back to for loop !!!
                        bListA->erase(lcA, rm);
                        return cellNow;
                    }
                    else {
                        lcAit++; // !!! Don't put it back to for loop !!!
                        continue;
                    }
                }
                validMaxGainA--;
                // cout << "validMaxGainA becomes "<< validMaxGainA<<endl;
                // cout <<"Now, its empty" << endl;
            }
            else{
                validMaxGainA--;
                // cout << "validMaxGainA becomes "<< validMaxGainA<<endl;
                continue;
            }
        }else{
            if(validMaxGainB == -1) continue; // different from validMaxGainA: possibility: A=3, B=-1
            listCell& lcB = bListB->getMaxGainList(validMaxGainB);
            // cout << "validMaxGainB: " << validMaxGainB << endl;
            if(!lcB.empty()){
                // cout << "lcB not empty" << endl;
                for(listCellIter lcBit = lcB.begin(); lcBit != lcB.end(); ){
                    if(tryPutOn((*lcBit)->lib, false)){
                        // cout << "Sucessfully put cell " << cellNow->crid << " on B" <<endl; 
                        listCellIter rm = lcBit;
                        cell* cellNow = *lcBit;
                        lcBit++; // !!! Don't put it back to for loop !!!
                        bListB->erase(lcB, rm);
                        return cellNow;
                    }
                    else {
                        lcBit++; // !!! Don't put it back to for loop !!!
                        continue;
                    }
                }
                validMaxGainB--;
                // cout << "validMaxGainB becomes "<< validMaxGainB<<endl;
                // cout <<"Now, its empty" << endl;
            }
            else{
                validMaxGainB--;
                // cout << "validMaxGainB becomes "<< validMaxGainB<<endl;
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
Move the base cell and update neighbor's gains
 */
void updateGain(cell* baseCell){
    for (auto n : baseCell->nets){
        baseCell->lock = true;
        int F, T;
        if(baseCell->part==true)
            F = n->distr.first, T = n->distr.second;
        else
            F = n->distr.second, T = n->distr.first;
        
        int cnt = 0;
        cell* leftedOne;
        /* before move */
        if(T==0) {
            for(auto c: n->cells){
                if(!c->lock) c->gain++;
            }
            // baseCell gain: original -1, now +1, should be ++ one more times
            baseCell->gain += 1;
        }
        else if(T==1){
            for(auto c: n->cells){
                if(baseCell->part != c->part and !c->lock and cnt < 2){
                    cnt++, leftedOne = c;
                }
            }
            if(cnt==1) leftedOne->gain--;
        }
        /* Move */
        F--, T++;
        baseCell->part = !baseCell->part;
        cnt = 0;
        /* after move */
        if(F==0){
            for(auto c: n->cells){
                if(!c->lock) c->gain--; 
            }
            // baseCell gain: original +1, now -1, should be -- one more times
            baseCell->gain -= 1;
        }
        else if(F==1){
            for(auto c: n->cells){
                if(baseCell->part != c->part and !c->lock and cnt < 2){
                    cnt++, leftedOne = c;
                }
                if(cnt == 1) leftedOne->gain++;
            }
        }

    }
    
}

void pass(){
    // cout << "pass()" << endl;

    cell* baseCell;
    init_distribution();
    init_cellGain();
    while(baseCell = maxGainCell()){
        // cout << baseCell->gain << " ";
        // cout << "Enter pass's while loop" << endl;
        Gk += baseCell->gain;
        // cout << "Gk: " << Gk << endl;
        steps.emplace_back(baseCell->gain);
        // cout << "update gain" << endl;
        updateGain(baseCell);
        // cout << "Done update gain" << endl;
    }
    // cout << endl;
    // cout << "End pass()" << endl;
    

}

int getBestMove(){
    int k = steps.size();
    int prefix[k] = {0};
    prefix[0] = steps[0];
    for(int i = 1; i < k; i++){
        prefix[i] = prefix[i-1] + steps[i];
    }
    
    int bestSeqIdx=0, m = -1;
    for(int i = 0; i < k; i++){
        if(m < prefix[i]) {
            bestSeqIdx = i;
            m = prefix[i];
        }
    }
    // cout << "Gain seq: ";
    // for(int i = 0; i<k; i++) cout << steps[i] << " ";
    // cout << "\nreturn best move index " << bestSeqIdx << endl;
    return m;
}

void FM(){
    // cout << "FM()" << endl;
    while(1){
        pass();
        cout << "Gk: " << Gk << endl;
        int moveTo = getBestMove();
        for(int i = steps.size()-1; i>moveTo; i--){
            cell* c = cellArray[steps[i]];
            c->part = !c->part;
        }
        Gk = 0;
        steps = {};
        if(Gk<=0){
            break;
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
    // cout << "output file!!"<<endl;
    // init_bucketList();

    output(outputPath);

    if (TIME){
        cout << "-------------------------------" << endl;
        printf("Total exe Time = %f\n", ((double)(clock() - start)) / CLOCKS_PER_SEC);
    }
}
