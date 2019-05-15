#pragma once

#include <string>
#include <iomanip>
#include <cstdio>
#include <sys/stat.h>
#include "fsmon.hpp"

using namespace std;

string fd2name(const int &x) {
    if (x == fileno(stdin)) return "\"<STDIN>\"";
    if (x == fileno(stdout)) return "\"<STDOUT>\"";
    if (x == fileno(stderr)) return "\"<STDERR>\"";
    string tmp = readlinkfd(x);
    return "\"" + tmp + "\"";
}

string i2h(auto &x) {
    string tmp;
    stringstream ss;
    ss << hex << x;
    ss >> tmp;
    return "0x" + tmp;
}

string i2o(auto &x) {
    string tmp;
    stringstream ss;
    ss << oct << x;
    ss >> tmp;
    return "0" + tmp;
}

string auto2s(const string &t, const void *x) {
    if (x == NULL) return "NULL";
    auto tmp = (long long unsigned int) x;
    return i2h(tmp);
}

string auto2s(const string &t, const char *x) {
    if (x == NULL) return "NULL";
    string tmp(x);
    return "\"" + tmp + "\"";
}

string auto2s(const string &t, FILE *x) {
    if (x == NULL) return "NULL";
    return fd2name(fileno(x));
}

string auto2s(const string &t, DIR *x) {
    if (x == NULL) return "NULL";
    return fd2name(dirfd(x));
}

string auto2s(const string &t, struct dirent *x) {
    if (x == NULL) return "NULL";
    string tmp(x->d_name);
    return "\"" + tmp + "\"";
}

string auto2s(const string &t, struct stat *x) {
    if (x == NULL) return "NULL";
    auto tmp = (unsigned long) x->st_mode;
    string mode = i2o(tmp);
    string sz = to_string((long long) x->st_size);
    string addr = auto2s(t, (void *) x);
    return addr + " {mode=" + mode + ", size=" + sz + "}";
}

string auto2s(const string &t, int x) {
    if (t == "FD") return fd2name(x);
    return to_string(x);
}

string auto2s(const string &t, unsigned int x) {
    if (t == "mode_t") return i2o(x);
    return to_string(x);
}

string auto2s(const string &t, long int x) {
    return to_string(x);
}

string auto2s(const string &t, long unsigned int x) {
    return to_string(x);
}
