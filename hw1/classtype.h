#include <vector>
#include <string>
#include <bitset>
#include <algorithm>
#include <set>
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

struct COLUMN;
struct COLUMN {

    string OnString;
    int OnNum;
    int length;
    set<string> PrimeImplicants;

    string IntToBinaryString(int num);
    bool AppendPrimeImplicantsAndCheckIfItIsResult(vector<string>& Primes);
    bool IsSubset (string imp);

    COLUMN (int VarNum, int OnNum) : length(VarNum), OnNum(OnNum) {
        OnString = IntToBinaryString(OnNum);
    };
};

struct Petrick;
struct Petrick {
    vector<COLUMN*> POS;
    vector<string> PrimeImplicants;
    vector<string> Answer;
    int length;

    void GetPOS(const vector<int>& Ons);
    void GetSOP();
    void Recursive();

    Petrick (int VarNum, const vector<int>& Ons, vector<string>& PrimeImplicants) : length(VarNum), PrimeImplicants(PrimeImplicants) {
        GetPOS(Ons);
    }

};