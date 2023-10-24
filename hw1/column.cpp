#include "classtype.h"
#include <string>
#include <vector>
using namespace std;

string COLUMN::IntToBinaryString(int num) {
    string binaryStr;
    for (int i = this->length - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        binaryStr.push_back('0' + bit);
    }
    return binaryStr;
}

bool COLUMN::AppendPrimeImplicantsAndCheckIfItIsResult(vector<ROW*>& Primes) {
    for (int i=0; i<Primes.size(); i++) {
        if (IsSubset(Primes[i]->PrimeImplicant)) {
            PrimeImplicants.insert(Primes[i]);
            Primes[i]->OnSet.push_back(this);
        }
    }
    if (PrimeImplicants.size() == 1) {
        return true;
    }
    else return false;
}

bool COLUMN::IsSubset (string imp) {
    for (int i=0; i<length; i++) {
        if (((OnString[i] == '1') && (imp[i] == '0')) || ((OnString[i] == '0') && (imp[i] == '1'))) {
            return false;
        }
    }
    return true;
}