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

    BigInt operator+(const BigInt &b) const {
        if (sign != b.sign) return *this - (-b);
        BigInt res; res.digits.clear();
        res.sign = sign;
        int carry = 0;
        int n = max(digits.size(), b.digits.size());
        for (int i = 0; i < n; i++) {
            int x = (i < digits.size()? digits[i]:0) + (i < b.digits.size()? b.digits[i]:0) + carry;
            res.digits.push_back(x%10); carry=x/10;
        }
        if (carry) res.digits.push_back(carry);
        return res;
    }

    BigInt operator-() const {
        BigInt res = *this; res.sign *= -1; return res;
    }

    BigInt operator-(const BigInt &b) const {
        if (sign != b.sign) return *this + (-b);
        if (*this < b) return -(b - *this);
        BigInt res; res.digits.clear();
        res.sign = sign;
        int borrow = 0;
        for (size_t i = 0; i < digits.size(); i++) {
            int x = digits[i] - (i < b.digits.size()? b.digits[i]:0) - borrow;
            if (x < 0) { x += 10; borrow = 1; } else borrow = 0;
            res.digits.push_back(x);
        }
        while (res.digits.size()>1 && res.digits.back()==0) res.digits.pop_back();
        return res;
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

    bool operator<(const BigInt &b) const {
        if (sign != b.sign) return sign < b.sign;
        if (digits.size() != b.digits.size()) return digits.size()*sign < b.digits.size()*b.sign;
        for (int i = digits.size()-1; i>=0; i--) if (digits[i]!=b.digits[i]) return digits[i]*sign < b.digits[i]*sign;
        return false;
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

    // polynomial coefficients
    vector<BigInt> coeffs={BigInt(1)};
    for(auto r: selected){
        vector<BigInt> newCoeffs(coeffs.size()+1,BigInt(0));
        for(size_t i=0;i<coeffs.size();i++){
            newCoeffs[i+1]=newCoeffs[i+1]+coeffs[i];      // multiply by x
            newCoeffs[i]=newCoeffs[i]-coeffs[i]*r;        // multiply by -r
        }
        coeffs=newCoeffs;
    }

    // output
    cout<<"n = "<<n<<", k = "<<k<<"\n";
    cout<<"Roots in decimal:\n";
    for(auto r:selected) cout<<r<<"\n";

    cout<<"Polynomial coefficients (highest degree first):\n";
    for(int i=(int)coeffs.size()-1;i>=0;i--) cout<<coeffs[i]<<" ";
    cout<<"\n";

    return 0;
}
