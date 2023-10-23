#include <vector>
#include <string>
#include <bitset>
#include <algorithm>
using namespace std;

struct QuineMcCluskey;
struct Petrick;

struct QuineMcCluskey {
    vector<string> Implicants;
    vector<string> PrimeImplicants;
    vector<int> OnSet;
    vector<int> DcSet;
    int length;

    string IntToBinaryString(int num);
    vector<string> ConvertAndSort();
    string CheckAndMerge(const string& imp1, const string& imp2);
    vector<string> GeneratePrimImplicants();

    QuineMcCluskey(int Varnum, const vector<int>& OnSet, const vector<int>& DcSet) : length(Varnum),  OnSet(OnSet), DcSet(DcSet) {
        Implicants = ConvertAndSort();
    };
};

struct Petrick {

};
