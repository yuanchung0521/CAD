#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

struct PINTYPE;
struct CELLTYPE;
struct LIB;
struct GATE;
struct MODULE;
struct NET;

struct PINTYPE {
    string Name;
    bool Input;
    float Capacitance;
    PINTYPE(string name, bool input) : Name(name), Input(input) {};
};

struct CELLTYPE {
    LIB* Lib;
    string Name;
    vector<PINTYPE*> InputPinList;
    PINTYPE* OutputPin;
    vector<vector<float>> PowerRiseTable, PowerFallTable;
    vector<vector<float>> CellRiseTable, CellFallTable, TransitionRiseTable, TransitionFallTable;
    CELLTYPE(string cellname) : Name(cellname) {}
    PINTYPE* NewPinType(string name, bool input) {
        PINTYPE* pintype = new PINTYPE(name, input);
        if (input) InputPinList.push_back(pintype);
        else OutputPin = pintype;
        return pintype;
    }
    
};

struct LIB {
    string TimeUnit, VoltageUnit, CurrentUnit, CapacitiveLoadUnit, Variable1, Variable2;
    vector<float> Index1, Index2;
    map<string, CELLTYPE*> CellTypeList;
    CELLTYPE* NewCellType (string cellname) {
        CELLTYPE* celltype = new CELLTYPE(cellname);
        CellTypeList.insert(pair<string, CELLTYPE*>(cellname, celltype));
        return celltype;
    }
};

struct GATE {
    CELLTYPE* Type;
    string Name;
    string Output;
    vector<string> Inputs;
};

struct MODULE {
    string Name;
    map<string, NET*> Netlist;
    vector<string> AllSignals, InputSignals, OutputSignals, WireSignals;
    vector<GATE*> GateList;
};

struct NET {
    string Name;
    vector<GATE*> GateInputList; // gate out == net in 
    vector<GATE*> GateOutputList; // gate in == net out

    NET (string name, int type) : Name(name) {}
};