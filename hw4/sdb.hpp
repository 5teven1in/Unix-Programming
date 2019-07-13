#pragma once

#include <string>
#include <sstream>
#include <vector>

#define PB push_back
#define NOLOAD  0
#define LOADED  1
#define RUNNING 2
#define MAXASM  0x100

using namespace std;

typedef vector<string> VS;
typedef long long LL;
typedef unsigned char UC;

struct breakpoint {
    int id;
    LL addr;
    UC ori;
    bool isfix;
    breakpoint(int _i = -1, LL _a = 0, UC _o = '\0', bool _f = false)
        : id(_i), addr(_a), ori(_o), isfix(_f) {
        }
};

VS split(const string &s, const char d = '\0') {
    VS res;
    stringstream ss(s);
    string item;
    if (d)
        while (getline(ss, item, d)) res.PB(item);
    else
        while (ss >> item) res.PB(item);
    return res;
}

string get_byte(const UC *byte) {
    stringstream ss;
    ss << hex << setfill('0') << setw(2) << (int) *byte;
    string tmp;
    ss >> tmp;
    return tmp;
}

string get_bytes(const UC *bytes, int n) {
    string out = "";
    bool fir = true;
    for (int i = 0; i < n; i++) {
        if (!fir) out += " ";
        fir = false;
        out += get_byte(bytes + i);
    }
    return out;
}

string get_printable(const string &s) {
    string out = "|";
    for (auto &x : s) {
        int tmp = x;
        if (tmp < 32 || tmp > 126) out += ".";
        else out += x;
    }
    out += "|";
    return out;
}

string flags2rwx(const LL flags) {
    string per = "xwr", tmp = "";
    for (int i = 2; i >= 0; i--) {
        if (flags & (1 << i)) tmp += per[i];
        else tmp += "-";
    }
    return tmp;
}

LL str2ll(const string &s) {
    if (s.find("0x") == 0 || s.find("0X") == 0) {
        return stoll(s, NULL, 16);
    }
    else if (s.find("0") == 0) {
        return stoll(s, NULL, 8);
    }
    else {
        return stoll(s);
    }
}

void get_code();
void get_regs();
string get_mem(const LL addr);
void print_reg(const string &name);
UC patch_byte(const LL addr, UC c);
bool isintext(const LL addr);
bool chkat(const auto &x, unsigned int at, bool p);
int chkst();
string disone(UC *pos, LL &addr);

void bp(const LL addr);
void cont();
void del(int id);
void disasm();
void dump(int sz = 80);
void quit();
void get(const string &reg);
void getregs();
void help();
void list();
void load();
void run();
void vmmap();
void set(const string &reg, LL val);
void si();
void start();
