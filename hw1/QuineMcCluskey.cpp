#include "classtype.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string QuineMcCluskey::IntToBinaryString(int num) {
    string binaryStr;
    for (int i = this->length - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        binaryStr.push_back('0' + bit);
    }
    return binaryStr;
}

bool compareByOnes(const string& a, const string& b) {
    int countA = count(a.begin(), a.end(), '1');
    int countB = count(b.begin(), b.end(), '1');

    return countA < countB;
}

vector<string> QuineMcCluskey::ConvertAndSort() {
    vector<string> binaryStrings;

    for (int num : this->OnSet) {
        string binaryStr = IntToBinaryString(num);
        binaryStrings.push_back(binaryStr);
    }

    for (int num : this->DcSet) {
        string binaryStr = IntToBinaryString(num);
        binaryStrings.push_back(binaryStr);
    }

    sort(binaryStrings.begin(), binaryStrings.end());
    sort(binaryStrings.begin(), binaryStrings.end(), compareByOnes);
    for (string s : binaryStrings) {
        cout << s << endl;
    }
    return binaryStrings;
}

string QuineMcCluskey::CheckAndMerge(const string& imp1, const string& imp2) {
    int distance = 0;
    string s;
    for (int i = 0; i < imp1.size(); i++) {
        if (imp1[i] != imp2[i]) {
            distance++;
            s += 'X';
        }
        else s += imp1[i];
    }
    return (distance == 1) ? s : "no";
}

vector<string> QuineMcCluskey::GeneratePrimImplicants() {
    cout << "Prime Implicants:" << endl;
    while (!this->Implicants.empty()) {
        vector<string> newImplicants;
        vector<bool> used(Implicants.size(), false);

        for (int i = 0; i < Implicants.size(); i++) {
            for (int j = i + 1; j < Implicants.size(); j++) {
                string merged = CheckAndMerge(Implicants[i], Implicants[j]);
                
                if (!(merged[0] == 'n')) {
                    if(find(newImplicants.begin() , newImplicants.end(), merged) == newImplicants.end()) {
                        newImplicants.push_back(merged);
                    }
                    used[i] = used[j] = true;
                }
            }

            if (!used[i]) {
                this->PrimeImplicants.push_back(Implicants[i]);
                cout << Implicants[i] << endl;
            }
        }
        Implicants.clear();
        Implicants = newImplicants;
    }
    return this->PrimeImplicants;
}