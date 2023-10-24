#include "classtype.h"
#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;

int main (int argc, char *argv[]) {

    ifstream InputFile(argv[1]);
    // ofstream OutputFile(argv[2]);

    int VarNum;
    string s;
    vector<int> OnSet;
    vector<int> DcSet;
    vector<bool> display(3);
    vector<string> PrimeImplicants;
    // Input data
    display[0] = false;
    while (InputFile >> s) {
        if (s == ".i") {
            InputFile >> s;
            VarNum = stoi(s);
            if (display[0]) cout << "VarNum = " << VarNum << endl;
        }
        else if (s == ".m") {
            InputFile >> s;
            if (display[0]) cout << "On : ";
            while (s != ".d") {
                if (display[0]) cout << s << " ";
                OnSet.push_back(stoi(s));
                InputFile >> s;
            }
            if (display[0]) cout << endl << "Don't care : ";
        }
        else {
            if (display[0]) cout << s << " ";
            DcSet.push_back(stoi(s));
        }
    }
    if (display[0]) cout << endl;
    
    QuineMcCluskey QMC(VarNum, OnSet, DcSet);
    PrimeImplicants = QMC.GeneratePrimImplicants();

    Petrick P(VarNum, OnSet, PrimeImplicants);
    P.GetSOP();
    return 0;

}
