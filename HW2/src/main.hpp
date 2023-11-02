#include <iostream>
// #include <List>
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
// class List;

// typedef std::vector<std::List<Cell*>>::iterator bListIterator;
// typedef List<Cell*>::iterator ListCellIter;
template <typename T>
class List {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int Size = 0;
    List() : head(nullptr), tail(nullptr), Size(0) {}

    int size(){
        return Size;
    }
    // Function to insert a new element at the end of the List
    void pushBack(const T& value) {
        Node* newNode = new Node(value);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        ++Size;
    }

    // Function to print the List
    void printList() {
        Node* current = head;
        while (current != nullptr) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    Node* deleteNode(Node* nodeToDelete) {
        if (nodeToDelete == nullptr) {
            return nullptr;  // Nothing to delete
        }

        Node* nextNode = nodeToDelete->next;  // Store the next node pointer
        if (nodeToDelete == head) {
            head = nodeToDelete->next;
        }

        if (nodeToDelete == tail) {
            tail = nodeToDelete->prev;
        }

        if (nodeToDelete->prev) {
            nodeToDelete->prev->next = nodeToDelete->next;
        }

        if (nodeToDelete->next) {
            nodeToDelete->next->prev = nodeToDelete->prev;
        }
        --Size;
        delete nodeToDelete;
        return nextNode;
    }

    // Destructor to free the memory
    ~List() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    class Iterator {
    public:
        Iterator(Node* node) : currentNode(node) {}

        T& operator*() {
            return currentNode->data;
        }
        Node* getNodePointer() {
            return currentNode;
        }
        Iterator& operator--() {
            return currentNode;
        }

        Iterator& operator++() {
            currentNode = currentNode->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator!=(const Iterator& other) {
            return currentNode != other.currentNode;
        }

    private:
        Node* currentNode;
    };

    Iterator begin() {
        return Iterator(head);
    }

    Iterator end() {
        return Iterator(nullptr);
    }
};

typedef List<Cell*> ListCell;

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
    List<Net*> nets;
    // cellNum* c;
    int lib; // Cell libarary virtual id
    bool lock; // true=locked, false=unlock
    bool part; // partition A(true) or B(false)
    int gain;
    int crid; // just for debug (Cell real id)
    int cvid;
    // List<Cell*>::iterator cellIt;
    List<Cell*>::Node* cellPtr;
    Cell(int _lib, int _cvid, int _crid){
        lib = _lib;
        cvid = _cvid;
        crid = _crid;
        lock = false;
        gain = 0;
    }
};


class Net{
public:
    List<Cell*> cells;
    int numCells; // May be redundant: can be get by cells.size()
    int nrid; // Net real id
    pair<int,int> distr; // for Net i, # of Cell in partition A or B:<A(i), B(i)>
    Net(int _numCells){
        numCells = _numCells;
    }
};

class BucketList{
public:
    int Pmax, maxGain;
    // int cellNum;
    vector<List<Cell*>> gainList;
    BucketList(int _Pmax){
        Pmax = _Pmax;
        int bListLen = Pmax * 2 + 1; // -Pmax ~ Pmax, 0 included
        maxGain = bListLen - 1; // point to top of gain List
        for (size_t i = 0; i < bListLen; ++i) {
            gainList.emplace_back(List<Cell*>()); // Create and push an empty list
        }
        // gainList.resize(bListLen);
        // cellNum = 0;
    }

    void insert(int gainValue, Cell* c){
        int idx = gainValue + Pmax;
        gainList[idx].pushBack(c);
        // cout << "inserting" << endl;
        c->cellPtr = gainList[idx].tail;
        c->gain = gainValue;
        // cellNum++;
    }
    List<Cell*>::Node* erase(ListCell &LC, List<Cell*>::Node* del) {
        return LC.deleteNode(del);
        // auto nextIter = LC.erase(del);
        // cellNum--;
        // return nextIter;
    }
    
    // int size(){
    //     return cellNum;
    // }
    List<Cell*> &getMaxGainList(int validMaxGainA){
        return gainList[validMaxGainA];
    }
    // void show(bool die){
    //     cout << "In BucketList " << (die?'A':'B') <<endl;
    //     cout << "Pmax: " << Pmax << endl;
    //     cout << "maxGain: " << maxGain <<" (index)" << endl;
    //     for(int i = 0; i<gainList.size(); i++){
    //         cout << "Gain "<< i-Pmax << ": "<<endl;
    //         for(auto c: gainList[i]){
    //             cout << c->crid <<((c != (gainList[i].back()))?" ":"\n");
    //         }
    //     }
    //     cout << endl << endl;
    // }
    void clear(){
        for(auto &L: gainList){
            L = List<Cell*>();
        }
        // cellNum = 0;
    }
};

