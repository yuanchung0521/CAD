#include "classtype.h"
#include <iostream>
#include <fstream>
#include <string>
#include <set>

using namespace std;

string ToAlphabet (string &imp) {
    string out;
    char tmp;
    for (int i=0; i<imp.size(); i++) {
        if (imp[i] != 'X') {
            tmp = 'A' + i;
            out += tmp;
            if (imp[i] == '0') {
                out += '\'';
            }
        }
    }
    return out;
}

bool customCompare(const string& str1, const string& str2) {
    for (int i = 0; i < min(str1.length(), str2.length()); ++i) {
        char char1 = str1[i];
        char char2 = str2[i];
        
        if (char1 == '\'' && char2 == '\'') {
            continue;
        }
        
        if (char1 == '\'') {
            return false;
        }
        
        if (char2 == '\'') {
            return true;
        }
        
        if (char1 != char2) {
            return char1 < char2;
        }
    }
    
    return str1 < str2;
}

int main (int argc, char *argv[]) {

    ifstream InputFile(argv[1]);
    ofstream OutputFile(argv[2]);

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

    vector<string> PrimeImplicantsInAlphabet;
    vector<string> AnswerInAlphabet;
    int count = 0;

    QuineMcCluskey QMC(VarNum, OnSet, DcSet);
    PrimeImplicants = QMC.GeneratePrimImplicants();

    for (string imp : PrimeImplicants) {
        PrimeImplicantsInAlphabet.push_back(ToAlphabet(imp));
    }
    sort(PrimeImplicantsInAlphabet.begin(), PrimeImplicantsInAlphabet.end(), customCompare);
    
    OutputFile << ".p " << PrimeImplicantsInAlphabet.size() << endl;
    for (string imp : PrimeImplicantsInAlphabet) {
        if (count < 15) {
            OutputFile << imp << endl;
        }
        count ++;
    }
    Petrick P(VarNum, OnSet, PrimeImplicants);
    P.GetSOP();

    for (string imp : P.Answer) {
        AnswerInAlphabet.push_back(ToAlphabet(imp));
    }
    sort(AnswerInAlphabet.begin(), AnswerInAlphabet.end(), customCompare);

    OutputFile << endl << ".mc " << AnswerInAlphabet.size() << endl;
    for (string imp : AnswerInAlphabet) OutputFile << imp << endl;
    OutputFile << "literal=" << P.MinLiteralsNum;
    
    return 0;
}
