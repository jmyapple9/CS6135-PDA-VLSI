#include <iostream>
#include <set>


using namespace std;

class die;
class cell;
class cellNum;
class net;
// class netNum;


class die{
public:
    int w, h;
    float utilA, utilB;
    char techA, techB;
};

class cell{
public:
    set<int> nets;
    int lib;
    cellNum* c;
    cell(){ // dummy node constructor
        c = nullptr;
        lib = -1;
    }
    cell(int _lib){
        lib = _lib;
    }
};

class cellNum{
public:
    cellNum *prev, *next;
    int id;
};

class net{
public:
    set<int> cells;
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