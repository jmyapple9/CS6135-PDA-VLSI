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
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace std;
using namespace boost::multiprecision;

class die;
class cell;
class cellNum;
class net;
// class netNum;


class die{
public:
    int w, h;
    double utilA, utilB;
    char techA, techB;
    int size;
    int Aarea, Barea;
    die(){
        Aarea = Barea = 0;
    }
    // int w, h;
    // cpp_dec_float_50 utilA, utilB;
    // char techA, techB;
    // cpp_int size;
    // cpp_int Aarea, Barea;
    // die(){
    //     Aarea = Barea = 0;
    // }
};

class cell{
public:
    list<int> nets;
    cellNum* c;
    int lib; // cell libarary virtual id
    bool lock; // true=locked, false=unlock
    char part; // partition A or B
    cell(){ // dummy node constructor
        c = nullptr;
        lib = -1;
    }
    cell(int _lib){
        lib = _lib;
        lock = false;
    }
};

class cellNum{
public:
    cellNum *prev, *next;
    int id;
};

class net{
public:
    list<int> cells;
    int numCells; // May be redundant: can be get by cells.size()
    net(){ // dummy node constructor
        numCells = -1;
    }
    net(int _numCells){
        numCells = _numCells;
    }
};

// class netNum{
// public:
//     netNum *prev, *next;
//     int id;
// };