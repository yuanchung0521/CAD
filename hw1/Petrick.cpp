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
        col->IsSol = col->AppendPrimeImplicantsAndCheckIfItIsResult(PrimeImplicants);
        POS.push_back(col);
        index++;
    }
    for (COLUMN* col : POS) {
        if (col->IsSol) {
            if (MustUse.find((*col->PrimeImplicants.begin())->PrimeImplicant) == MustUse.end()) {
                MustUse.insert((*col->PrimeImplicants.begin())->PrimeImplicant);
                MustUseLiteralsNum += (*col->PrimeImplicants.begin())->literals;
            }
            for (COLUMN* i : (*col->PrimeImplicants.begin())->OnSet) {
                CoveredIndex.push_back(i->index);
            }
            col->PrimeImplicants.clear();
        }
    }
    for (int bye : CoveredIndex) {
        POS[bye]->PrimeImplicants.clear();
    }
}

void Petrick::GetSOP() {
    set<string> P;
	Recursive(P,0);
    for (auto &imp : MustUse) Answer.push_back(imp);
    for (string imp : Try) Answer.push_back(imp);
    MinLiteralsNum += MustUseLiteralsNum;
    cout << "prime imp= " << Answer.size() << endl;
    cout << "literal= " << MinLiteralsNum << endl;

}

void Petrick::Recursive(set<string> &P, int i) {
    
    if (i == POS.size()) { // find one set of SOP, which might be an answer
        if (P.size() < MinPrimeImplicantsNum)
        if (NowLiteralsNum < MinLiteralsNum) {
            Try.clear();
            for (auto &s : P) {
                Try.push_back(s);
            }
            MinLiteralsNum = NowLiteralsNum;
            MinPrimeImplicantsNum = Try.size();
        }
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