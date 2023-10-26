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
class Cell;
class Net;
class Die;

typedef std::vector<std::list<Cell*>>::iterator bListIterator;
typedef list<Cell*>::iterator listCellIter;
typedef list<Cell*> listCell;

class Die{
public:
    long long w, h;
    // double utilA, utilB;
    long long availA, availB;
    long long size;
    long long Aarea, Barea;
    char techA, techB;
    Die(){
        Aarea = Barea = 0;
    }
};

class Cell{
public:
    list<Net*> nets;
    // cellNum* c;
    int lib; // Cell libarary virtual id
    bool lock; // true=locked, false=unlock
    bool part; // partition A(true) or B(false)
    int gain;
    int crid; // just for debug (Cell real id)
    int cvid;
    list<Cell*>::iterator cellIt;
    Cell(int _lib, int _cvid){
        lib = _lib;
        cvid = _cvid;
        lock = false;
        gain = 0;
    }
};


class Net{
public:
    list<Cell*> cells;
    int numCells; // May be redundant: can be get by cells.size()
    int nrid; // Net real id
    pair<int,int> distr; // for Net i, # of Cell in partition A or B:<A(i), B(i)>
    Net(int _numCells){
        numCells = _numCells;
    }
};

class BucketList{
public:
    int Pmax, maxGain, cellNum;
    vector<list<Cell*>> gainList;
    BucketList(int _Pmax){
        Pmax = _Pmax;
        int bListLen = Pmax * 2 + 1; // -Pmax ~ Pmax, 0 included
        maxGain = bListLen - 1; // point to top of gain list
        gainList.resize(bListLen);
        cellNum = 0;
    }

    void insert(int gainValue, Cell* c){
        int idx = gainValue + Pmax;
        gainList[idx].emplace_back(c);
        // cout << "inserting" << endl;
        listCellIter it = prev(gainList[idx].end());
        c->gain = gainValue;
        c->cellIt = it;
        cellNum++;
    }
    void erase(listCell &LC, listCellIter cit){
        LC.erase(cit);
        cellNum--;
    }
    int size(){
        return cellNum;
    }
    list<Cell*> &getMaxGainList(int validMaxGainA){
        return gainList[validMaxGainA];
    }
    void show(bool die){
        cout << "In Bucketlist " << (die?'A':'B') <<endl;
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
    void clear(){
        for(auto &L: gainList){
            L = list<Cell*>();
        }
        cellNum = 0;
    }
};