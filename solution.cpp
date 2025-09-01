#include <bits/stdc++.h>
using namespace std;

// --------- Minimal BigInt class (supports +, -, *, << for printing) ---------
struct BigInt {
    vector<int> digits; // little-endian
    int sign = 1;

    BigInt(long long x = 0) {
        if (x < 0) { sign = -1; x = -x; }
        if (x == 0) digits.push_back(0);
        while (x) { digits.push_back(x % 10); x /= 10; }
    }

    BigInt(const string &s) {
        sign = 1;
        int i = 0;
        if (s[0] == '-') { sign = -1; i = 1; }
        for (int j = s.size()-1; j >= i; j--) digits.push_back(s[j]-'0');
    }

    BigInt operator*(const BigInt &b) const {
        BigInt res; res.digits.assign(digits.size()+b.digits.size(),0); res.sign=sign*b.sign;
        for (size_t i = 0; i < digits.size(); i++)
            for (size_t j = 0; j < b.digits.size(); j++)
                res.digits[i+j] += digits[i]*b.digits[j];
        int carry = 0;
        for (size_t i = 0; i < res.digits.size(); i++) {
            res.digits[i] += carry; carry=res.digits[i]/10; res.digits[i]%=10;
        }
        while (res.digits.size()>1 && res.digits.back()==0) res.digits.pop_back();
        return res;
    }

    friend ostream& operator<<(ostream &os, const BigInt &b) {
        if (b.sign==-1) os << '-';
        for (int i = b.digits.size()-1; i>=0; i--) os << b.digits[i];
        return os;
    }
};

// --------- Helper to convert string in any base to BigInt ---------
BigInt toBigInt(const string &val, int base) {
    BigInt result(0), B(base);
    for (char c: val) {
        int digit;
        if (isdigit(c)) digit = c-'0';
        else if (isalpha(c)) digit = 10 + tolower(c)-'a';
        else continue;
        result = result * B + BigInt(digit);
    }
    return result;
}

// --------- Main ---------
string clean(const string &s) {
    string out;
    for (char c: s) if (!isspace(c) && c!='"' && c!=',') out.push_back(c);
    return out;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    string line, jsonInput;
    while (getline(cin,line)) jsonInput+=line;

    // extract n and k
    int n=0,k=0;
    size_t pos_n=jsonInput.find("\"n\"");
    if(pos_n!=string::npos){size_t colon=jsonInput.find(":",pos_n); size_t end=jsonInput.find_first_of(",}",colon); n=stoi(clean(jsonInput.substr(colon+1,end-colon-1)));}
    size_t pos_k=jsonInput.find("\"k\"");
    if(pos_k!=string::npos){size_t colon=jsonInput.find(":",pos_k); size_t end=jsonInput.find_first_of(",}",colon); k=stoi(clean(jsonInput.substr(colon+1,end-colon-1)));}

    // extract roots
    vector<BigInt> roots;
    for(int i=1;i<=n;i++){
        string key="\""+to_string(i)+"\"";
        size_t pos=jsonInput.find(key); if(pos==string::npos) continue;

        size_t pos_base=jsonInput.find("\"base\"",pos); size_t colon1=jsonInput.find(":",pos_base);
        size_t end1=jsonInput.find_first_of(",}",colon1); int base=stoi(clean(jsonInput.substr(colon1+1,end1-colon1-1)));

        size_t pos_val=jsonInput.find("\"value\"",pos); size_t colon2=jsonInput.find(":",pos_val);
        size_t start=jsonInput.find("\"",colon2)+1; size_t end=jsonInput.find("\"",start);
        string valStr=jsonInput.substr(start,end-start);

        roots.push_back(toBigInt(valStr,base));
    }

    if(roots.size()<k){cerr<<"Error: fewer roots than k\n"; return 1;}
    vector<BigInt> selected(roots.begin(),roots.begin()+k);

    // compute constant term
    BigInt constant(1);
    for(auto r: selected) constant = constant * BigInt(-1) * r;

    // output
    cout << constant << "\n";

    return 0;
}
