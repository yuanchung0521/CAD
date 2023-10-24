#include "classtype.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void Petrick::GetPOS(const vector<int>& Ons) {
    COLUMN *col;
    bool Is_Solution;
    for (int on : Ons) {
        col = new COLUMN(length, on);
        Is_Solution =  col->AppendPrimeImplicantsAndCheckIfItIsResult(PrimeImplicants);
        if (Is_Solution) {
            Answer.push_back(*col->PrimeImplicants.begin());
        }
        else {
            POS.push_back(col);
        }
    }
}

void Petrick::GetSOP() {

}