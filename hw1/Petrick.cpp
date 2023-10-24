#include "classtype.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void Petrick::GetPOS(const vector<int>& Ons) {
    COLUMN *col;
    ROW *row;
    bool Is_Solution;
    int index = 0;
    vector<int> CoveredIndex;
    for (int on : Ons) {
        col = new COLUMN(length, on, index);
        Is_Solution =  col->AppendPrimeImplicantsAndCheckIfItIsResult(PrimeImplicants);
        if (Is_Solution) {
            MustUse.push_back((*col->PrimeImplicants.begin())->PrimeImplicant);
            MustUseLiteralsNum += (*col->PrimeImplicants.begin())->literals;
            for (COLUMN* i : (*col->PrimeImplicants.begin())->OnSet) {
                CoveredIndex.push_back(i->index);
            }
            col->PrimeImplicants.clear();
            POS.push_back(col);
        }
        else {
            POS.push_back(col);
        }
        index++;
    }
    for (int bye : CoveredIndex) {
        POS[bye]->PrimeImplicants.clear();
    }
    for (COLUMN* col : POS) {
        cout << col->OnNum << ": ";
        for (ROW* row : col->PrimeImplicants) {
            cout << row->PrimeImplicant << " ";
        }
        cout << endl;
    }
}

void Petrick::GetSOP() {
    set<string> P;
	Recursive(P,0);
    cout << "answer: " << endl;
    for (string imp : Try) {
        cout << imp << " ";
    }
    cout << endl;
    for (string imp : MustUse) Answer.push_back(imp);
    for (string imp : Try) Answer.push_back(imp);
    MinLiteralsNum += MustUseLiteralsNum;
    for (string imp : Answer) cout << imp << " ";
    cout << endl;
    cout << "prime imp= " << Answer.size();
    cout << "literal= " << MinLiteralsNum;

}

void Petrick::Recursive(set<string> &P, int i) {
    cout << i << endl;
    if (i == POS.size()) { // find one set of SOP, which might be an answer
        for (auto &s : P) {
            cout << s << " ";
        }
        if (NowLiteralsNum < MinLiteralsNum) {
            Try.clear();
            for (auto &s : P) {
                Try.push_back(s);
            }
            MinLiteralsNum = NowLiteralsNum;
            MinPrimeImplicantsNum = Try.size();
        }
        cout << endl;
        return;
    }
    if (POS[i]->PrimeImplicants.empty()) {
        Recursive(P, i+1);   
    }
    else {
        for (ROW* imp : POS[i]->PrimeImplicants) {
            if (P.find(imp->PrimeImplicant) == P.end()) {
                if (P.size()+1 > MinPrimeImplicantsNum) return;
                else {
                    P.insert(imp-> PrimeImplicant);
                    NowLiteralsNum += imp->literals;
                    Recursive(P, i+1);
                    P.erase(imp->PrimeImplicant);
                    NowLiteralsNum -= imp->literals;
                }
                
            }
            else {
                Recursive(P, i+1);
            }
        }
    }
}