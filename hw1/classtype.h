#include <vector>
#include <string>
#include <bitset>
#include <algorithm>
#include <set>
using namespace std;

struct QuineMcCluskey;
struct Petrick;
struct COLUMN;
struct ROW;


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


struct COLUMN {

    string OnString;
    int OnNum;
    int length;
    int index;
    set<ROW*> PrimeImplicants;

    string IntToBinaryString(int num);
    bool AppendPrimeImplicantsAndCheckIfItIsResult(vector<ROW*>& Primes);
    bool IsSubset (string imp);

    COLUMN (int VarNum, int OnNum, int index) : length(VarNum), OnNum(OnNum), index(index) {
        OnString = IntToBinaryString(OnNum);
    };
};


struct ROW {
    string PrimeImplicant;
    vector<COLUMN*> OnSet;
    int literals;

    ROW (string imp) : PrimeImplicant(imp) {
        literals = 0;
        for (int i=0; i<imp.size(); i++) {
            if (imp[i] != 'X') literals++;
        }
    };
};

struct Petrick;
struct Petrick {
    vector<COLUMN*> POS;
    vector<ROW*> PrimeImplicants;
    vector<string> MustUse;
    vector<string> Try;
    vector<string> Answer;
    int length;
    int MinPrimeImplicantsNum;
    int MinLiteralsNum, NowLiteralsNum, MustUseLiteralsNum;

    void GetPOS(const vector<int>& Ons);
    void GetSOP();
    void Recursive(set<string> &P, int i);

    Petrick (int VarNum, const vector<int>& Ons, vector<string>& Primes) : length(VarNum), MinPrimeImplicantsNum(Primes.size()), MinLiteralsNum(VarNum*Primes.size()), NowLiteralsNum(0), MustUseLiteralsNum(0) {
        ROW *row;
        for (string imp : Primes) {
            row = new ROW(imp);
            PrimeImplicants.push_back(row);
        }
        GetPOS(Ons);
    }

};