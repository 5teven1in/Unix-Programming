#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <getopt.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define PB push_back

using namespace std;

typedef vector<string> VS;
typedef vector<int> VI;

char optstring[] = "tu";

struct option longopts[] = {
    {"tcp", no_argument, NULL, 't'},
    {"udp", no_argument, NULL, 'u'},
    {NULL, no_argument, NULL, 0}
};

bool is_v6 = false;
bool is_tcp = true;

string nettype[] = {"udp", "udp6", "tcp", "tcp6"};

struct netstat {
    int ty;
    VS s;
    void print() {
        cout << setiosflags(ios::left) << setw(6)
            << nettype[ty]
            << setw(24)
            << s[0] + ":" + s[1]
            << setw(24)
            << s[2] + ":" + s[3]
            << s[4] + "/" + s[5] << endl;
    }
};

vector<netstat> tcps, udps;
//   inode, pid
map<string, string> map2pid;
//     pid, cmd
map<string, string> map2cmd;
string filter;

void list(const string name) {
    cout << "List of " << name << " connections:" << endl;
    cout << setiosflags(ios::left) << setw(6)
        << "Proto" << setw(24)
        << "Local Address" << setw(24)
        << "Foreign Address" 
        << "PID/Program name and arguments" << endl;
}

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

void h2i(const string &s, auto &x) {
    stringstream ss;
    ss << hex << s;
    ss >> x;
}

bool recmp(const string &s, const string &pat) {
    regex re(pat);
    return regex_search(s, re);
}

string get_addr(const string &s) {
    if (is_v6) {
        struct sockaddr_in6 sa;
        for (int i = 0; i < 4; i++) {
            h2i(s.substr(i * 8, 8), sa.sin6_addr.s6_addr32[i]);
        }
        char str[INET6_ADDRSTRLEN + 1] = {0};
        inet_ntop(AF_INET6, &(sa.sin6_addr), str, INET6_ADDRSTRLEN);
        return str;
    }
    else {
        struct sockaddr_in sa;
        h2i(s, sa.sin_addr.s_addr);
        char str[INET_ADDRSTRLEN + 1] = {0};
        inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);
        return str;
    }
}

string get_port(const string &s) {
    int port;
    h2i(s, port);
    return port ? to_string(port) : "*";
}

VS convert(const string &s) {
    VS tmp = split(s, ':'), res;
    res.PB(get_addr(tmp[0]));
    res.PB(get_port(tmp[1]));
    return res;
}

VS list_dir(const string &path) {
    VS res;
    DIR *dirp = opendir(path.c_str());
    if (dirp == NULL) return res;
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL) {
        res.PB(dp->d_name);
    }
    closedir(dirp);
    return res;
}

int get_sockinode(const string &path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == -1) {
        return -1;
    }
    return (sb.st_mode & S_IFMT) == S_IFSOCK ? sb.st_ino : -1;
}

string get_cmd(const string &path) {
    ifstream fin(path);
    string cmd;
    getline(fin, cmd);
    for (auto &x : cmd) {
        if (x == '\0') x = ' ';
    }
    fin.close();
    return cmd;
}

void build_map() {
    string proc = "/proc/";
    VS pids = list_dir(proc);
    for (auto &pid : pids) {
        string fd_path = proc + pid + "/fd/";
        string cmd = get_cmd(proc + pid + "/cmdline");
        if (cmd == "") continue;
        map2cmd[pid] = cmd;
        VS items = list_dir(fd_path);
        for (auto &x : items) {
            int inode = get_sockinode(fd_path + x);
            if (inode != -1) {
                map2pid[to_string(inode)] = pid;
            }
        }
    }
}

void parse(const string &path) {
    ifstream fin(path);
    string tmp;
    getline(fin, tmp);      // don't need the first line
    while (getline(fin, tmp)) {
        VS v = split(tmp);
        VS lo = convert(v[1]), fo = convert(v[2]);
        string pid = map2pid[v[9]];
        string cmd = map2cmd[pid];
        netstat n{(is_tcp << 1) | is_v6, {lo[0], lo[1], fo[0], fo[1], pid, cmd}};
        is_tcp ? tcps.PB(n) : udps.PB(n);
    }
    fin.close();
}

void list_tcp() {
    is_tcp = true;
    is_v6 = false;
    parse("/proc/net/tcp");
    is_v6 = true;
    parse("/proc/net/tcp6");
    list("TCP");
    for (auto &x : tcps) {
        if (filter == "" || recmp(x.s[5], filter))
            x.print();
    }
    cout << endl;
}

void list_udp() {
    is_tcp = false;
    is_v6 = false;
    parse("/proc/net/udp");
    is_v6 = true;
    parse("/proc/net/udp6");
    list("UDP");
    for (auto &x : udps) {
        if (filter == "" || recmp(x.s[5], filter))
            x.print();
    }
    cout << endl;
}

int main(int argc, char* argv[]) {
    bool list_t = false, list_u = false;
    int c;
    while ((c = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (c) {
            case 't':
                list_t = true;
                break;
            case 'u':
                list_u = true;
                break;
            default:
                cout << "Usage: ./hw1 [-t|--tcp] [-u|--udp] [filter-string]" << endl;
                return 1;
        }
    }
    argc -= optind;
    argv += optind;
    if (argv[0] != NULL) filter = argv[0];
    build_map();
    if (list_t | list_u) {
        if (list_t) list_tcp();
        if (list_u) list_udp();
    }
    else {
        list_tcp();
        list_udp();
    }
    return 0;
}
