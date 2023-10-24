#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <time.h>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <iterator>
// #include <boost/multiprecision/cpp_int.hpp>
// #include <boost/multiprecision/cpp_dec_float.hpp>

using namespace std;
// using namespace boost::multiprecision;
/* 
useful regular expression to find "Net NX Y", where X, Y are numbers: Net N[\d]+ [\d]+
 */

class die;
class cell;
class net;

typedef std::vector<std::list<cell*>>::iterator bListIterator;
typedef list<cell*>::iterator listCellIter;
typedef list<cell*> listCell;

class die{
public:
    long long w, h;
    // double utilA, utilB;
    long long availA, availB;
    long long size;
    long long Aarea, Barea;
    char techA, techB;
    die(){
        Aarea = Barea = 0;
    }
};

class cell{
public:
    list<net*> nets;
    // cellNum* c;
    int lib; // cell libarary virtual id
    bool lock; // true=locked, false=unlock
    bool part; // partition A(true) or B(false)
    int gain;
    int crid; // just for debug (cell real id)
    int cvid;
    list<cell*>::iterator cellIt;
    cell(int _lib, int _cvid){
        lib = _lib;
        cvid = _cvid;
        lock = false;
        gain = 0;
    }
};


class net{
public:
    list<cell*> cells;
    int numCells; // May be redundant: can be get by cells.size()
    int nrid; // net real id
    pair<int,int> distr; // for net i, # of cell in partition A or B:<A(i), B(i)>
    net(int _numCells){
        numCells = _numCells;
    }
};

class bucketList{
public:
    int Pmax, maxGain, cellNum;
    vector<list<cell*>> gainList;
    bucketList(int _Pmax){
        Pmax = _Pmax;
        int bListLen = Pmax * 2 + 1; // -Pmax ~ Pmax, 0 included
        maxGain = bListLen - 1; // point to top of gain list
        gainList.resize(bListLen);
        cellNum = 0;
    }

    void insert(int gainValue, cell* c){
        int idx = gainValue + Pmax;
        gainList[idx].emplace_back(c);
        listCellIter it = prev(gainList[idx].end());
        c->gain = gainValue;
        c->cellIt = it;
        cellNum++;
    }
    void erase(listCell &LC, listCellIter cit){
        // cout << "before erase:";
        // for(auto c: LC) cout << c->crid <<" ";

        LC.erase(cit);
        cellNum--;
        // cout << "\nAfter erase:";
        // for(auto c: LC) cout << c->crid <<" ";
        // cout << endl;
    }
    int size(){
        return cellNum;
    }
    list<cell*> &getMaxGainList(int validMaxGainA){
        return gainList[validMaxGainA];
    }
    void show(char die){
        cout << "In bucketlist " << die <<endl;
        cout << "Pmax: " << Pmax << endl;
        cout << "maxGain: " << maxGain <<" (index)" << endl;
        for(int i = 0; i<gainList.size(); i++){
            cout << "Gain "<< i-Pmax << ": "<<endl;
            for(auto c: gainList[i]){
                cout << c->crid <<((c != (gainList[i].back()))?" ":"\n");
            }
        }
        cout << endl << endl;
    }
};