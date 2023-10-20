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
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace std;
using namespace boost::multiprecision;

class die;
class cell;
class net;


class die{
public:
    cpp_dec_float_50 w, h;
    cpp_dec_float_50 utilA, utilB;
    cpp_dec_float_50 size;
    cpp_dec_float_50 Aarea, Barea;
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
    char part; // partition A or B
    int gain;
    int crid; // just for debug (cell real id)
    list<cell*>::iterator cellIt;
    // cell(){ // dummy node constructor
    //     c = nullptr;
    //     lib = -1;
    // }
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
    int Pmax, maxGain;
    vector<list<cell*>> gainList;
    bucketList(int _Pmax){
        Pmax = _Pmax;
        int bListLen = Pmax * 2 + 1; // -Pmax ~ Pmax, 0 included
        gainList.resize(bListLen);
    }

    void insert(int gainValue, cell* c){
        int idx = gainValue + Pmax;
        gainList[idx].emplace_back(c);
        list<cell*>::iterator it = prev(gainList[idx].end());
        c->cellIt = it;
    }
    
};