#include <vector>
#include <string>
#include <bitset>
#include <algorithm>
using namespace std;

struct QuineMcCluskey;
struct QuineMcCluskey {
    vector<string> Implicants;
    vector<int> OnSet;
    vector<int> DcSet;
    int length;

    QuineMcCluskey(int Varnum, const vector<int>& OnSet, const vector<int>& DcSet) : length(Varnum),  OnSet(OnSet), DcSet(DcSet) {
        Implicants = ConvertAndSort();
    };

    string IntToBinaryString(int num);
    vector<string> ConvertAndSort();
    
};
