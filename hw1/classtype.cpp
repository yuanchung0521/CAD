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
    for (string s : binaryStrings) {
        cout << s << endl;
    }
    return binaryStrings;
}

