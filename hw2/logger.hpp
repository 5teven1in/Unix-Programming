#pragma once

#include <string>
#include <vector>
#include "fsmon.hpp"
#include "typestring.hpp"

using namespace std;

class Logger {
    public:
        Logger(const string &n, const bool iv)
            : name(n), isva(iv) {
            }
        Logger add(const string &t, auto &x) const {
            Logger l = *this;
            l.argv.push_back(auto2s(t, x));
            return l;
        }
        Logger addret(const string &t, auto &x) const {
            Logger l = *this;
            l.ret = auto2s(t, x);
            return l;
        }
        void log() {
            bool isfir = true;
            output("# %s(", name.c_str());
            for (auto &x : argv) {
                if (isfir) {
                    output("%s", x.c_str());
                    isfir = false;
                    continue;
                }
                output(", %s", x.c_str());
            }
            output(isva ? ", ...) = %s\n" : ") = %s\n", ret.c_str());
        }
    private:
        bool isva;
        string name, ret;
        vector<string> argv;
};
