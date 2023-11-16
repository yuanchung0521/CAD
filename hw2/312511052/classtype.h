#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

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
    double Capacitance;
    PINTYPE(string name, bool input) : Name(name), Input(input) {};
};

struct CELLTYPE {
    LIB* Lib;
    string Name;
    map<string, PINTYPE*> InputPinList;
    PINTYPE* OutputPin;
    vector<vector<double>> PowerRiseTable, PowerFallTable;
    vector<vector<double>> CellRiseTable, CellFallTable, TransitionRiseTable, TransitionFallTable;
    CELLTYPE(string cellname) : Name(cellname) {}
    PINTYPE* NewPinType(string name, bool input) {
        PINTYPE* pintype = new PINTYPE(name, input);
        if (input) InputPinList.insert(pair<string, PINTYPE*>(name, pintype));
        else OutputPin = pintype;
        return pintype;
    }
    
};

struct LIB {
    string TimeUnit, VoltageUnit, CurrentUnit, CapacitiveLoadUnit, Variable1, Variable2;
    vector<double> Index1, Index2;
    map<string, CELLTYPE*> CellTypeList;
    CELLTYPE* NewCellType (string cellname) {
        CELLTYPE* celltype = new CELLTYPE(cellname);
        CellTypeList.insert(pair<string, CELLTYPE*>(cellname, celltype));
        return celltype;
    }
};

struct NET {
    string Name;
    int Type; // 0 for input, 1 for output, 2 for wire
    GATE* GateInput; // gate out == net in 
    vector<GATE*> GateOutputList; // (gate name, (next gate input pin, next gate))

    NET (string name, int type) : Name(name), Type(type), GateInput(nullptr) {}
};

struct GATE {
    bool Ready, Done;
    CELLTYPE* Type;
    string Name;
    NET* Output;
    map<string, string> Inputs; // (netname, Input type name)
    map<string, GATE*> FanIn; // (gate input pin, prev gate)
    vector<pair<string, GATE*>> FanOut; // (next gate input pin, next gate)
    double ArriveTime, Delay, OutCap, InTran, TranDelay, MaxTime; // InTran is the input transisition delay use to cal Delay, TranDelay is this cell's tran delay the next gate uses
    int rise;
    GATE () : ArriveTime(0), Delay(0), OutCap(0), InTran(0), TranDelay(0), Ready(false), MaxTime(0), rise(0) {}
    void getOutCapAndInTran() {
        for (auto& gate : FanOut) {
            OutCap += gate.second->Type->InputPinList[gate.first]->Capacitance;
        }
        if (Output->Type == 1) OutCap += 0.03;

        for (auto& gate : FanIn) {
            if (gate.second->ArriveTime > MaxTime) {
                InTran = gate.second->TranDelay;
                MaxTime = gate.second->ArriveTime;
            }
        }
    }

    double Calculation (LIB* lib, vector<vector<double>>& tmpvec, int lowIndex1, int lowIndex2) {
        double C1 = lib->Index1[lowIndex1];
        double C2 = lib->Index1[lowIndex1+1];
        double S1 = lib->Index2[lowIndex2];
        double S2 = lib->Index2[lowIndex2+1];

        double P0 = tmpvec[lowIndex2][lowIndex1];
        double P2 = tmpvec[lowIndex2+1][lowIndex1];
        double P1 = tmpvec[lowIndex2+1][lowIndex1+1];
        double P3 = tmpvec[lowIndex2][lowIndex1+1];
        double A = P0 + (P2-P0)/(S2-S1)*(InTran-S1);
        double B = P3 + (P1-P3)/(S2-S1)*(InTran-S1);
        // cout << lowIndex1 << " " << lowIndex2 << endl;
        // cout << C1 << " " << C2 << " " << S1 << " " << S2 << " " << P0 << " " << P1 << " " << P2 << " " << P3 << endl;

        return A + (B-A)/(C2-C1)*(OutCap-C1);
    }

    void getDelay(LIB* lib) {
        getOutCapAndInTran();
        // cout << Name << " " << OutCap << " " << InTran << endl;
        int lowIndex1, lowIndex2;
        
        if (OutCap < lib->Index1[1]) lowIndex1 = 0;
        else if (OutCap >= lib->Index1[lib->Index1.size()-2]) lowIndex1 = lib->Index1.size()-2;
        else {
            for (int i=1; i<lib->Index1.size()-1; i++) {
                if (lib->Index1[i] > OutCap) {
                    lowIndex1 = i-1;
                    break;
                }
            }
        }

        if (InTran < lib->Index2[1]) lowIndex2 = 0;
        else if (InTran >= lib->Index2[lib->Index2.size()-2]) lowIndex2 = lib->Index2.size()-2;
        else {
            for (int i=1; i<lib->Index2.size()-1; i++) {
                if (lib->Index2[i] > InTran) {
                    lowIndex2 = i-1;
                    break;
                }
            }
        }
        // cout << "index: " << lowIndex1 << " " << lowIndex2 << endl;
        double RiseDelay = Calculation(lib, Type->CellRiseTable, lowIndex1, lowIndex2);
        double FallDelay = Calculation(lib, Type->CellFallTable, lowIndex1, lowIndex2);
        // cout << RiseDelay << " " << FallDelay << endl;
        Delay = (RiseDelay > FallDelay) ? RiseDelay : FallDelay;
        rise = (RiseDelay > FallDelay) ? 1 : 0;

        double RiseTran = Calculation(lib, Type->TransitionRiseTable, lowIndex1, lowIndex2);
        double FallTran = Calculation(lib, Type->TransitionFallTable, lowIndex1, lowIndex2);
        TranDelay = (RiseDelay > FallDelay) ? RiseTran : FallTran;
        
        for (auto& nextgate : FanOut) {
            bool allready = true;
            for (auto& nextgateprev : nextgate.second->FanIn) {
                if (!nextgateprev.second->Ready) allready = false;
            }
            if (allready) nextgate.second->Ready = true;
        }
        ArriveTime = MaxTime + Delay + 0.005;
        Done = true;
    }

};

struct MODULE {
    string Name;
    map<string, NET*> Netlist; // (netname, net*)
    vector<NET*> AllSignals, InputSignals, OutputSignals, WireSignals;
    vector<GATE*> GateList;
};

