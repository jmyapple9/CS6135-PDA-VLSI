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
    list<cell*>::iterator cellIt;
    cell(int _lib){
        lib = _lib;
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
        Pmax = maxGain = _Pmax;
        int bListLen = Pmax * 2 + 1; // -Pmax ~ Pmax, 0 included
        gainList.resize(bListLen);
        cellNum = 0;
    }

    void insert(int gainValue, cell* c){
        int idx = gainValue + Pmax;
        gainList[idx].emplace_back(c);
        listCellIter it = prev(gainList[idx].end());
        c->cellIt = it;
        cellNum++;
    }
    void erase(listCell LC, listCellIter cit){
        LC.erase(cit);
        cellNum--;
    }
    int size(){
        return cellNum;
    }
    list<cell*> &getMaxGainList(int validMaxGainA){
        return gainList[validMaxGainA];
    }
};