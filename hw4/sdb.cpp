#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <capstone/capstone.h>
#include "elftool.hpp"
#include "sdb.hpp"

using namespace std;

int state = NOLOAD;
elf_handle_t *eh = NULL;
elf_strtab_t *tab = NULL;
elf_shdr_t texts = {0};
elf_phdr_t tphdr = {0};
string progpath = "";
pid_t pid = 0;
LL disaddr = -1;
LL dumpaddr = -1;
LL bpaddr = -1;
LL textbase = 0;
char *code = NULL;
bool ispie = false;
int bpid = 0;
int dislen = 10;
int hitbp = -1;
struct user_regs_struct regs_struct = {0};
map<string, LL*> regs;
VS reglist = {"rax", "rbx", "rcx", "rdx",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    "rdi", "rsi", "rbp", "rsp", "rip", "flags"};
vector<breakpoint> bpoints;

void get_code() {
    ifstream f(progpath.c_str(), ios::in | ios::binary | ios::ate);
    streampos size;
    size = f.tellg();
    int codesz = size + 1L;
    code = new char [codesz];
    f.seekg(0, ios::beg);
    f.read(code, size);
    code[size] = 0;
    f.close();
}

string get_mem(const LL addr) {
    string s = "";
    for (int i = 0; i < MAXASM / 8; i++) {
        auto out = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
        s += string((char*) &out, 8);
    }
    return s;
}

void get_regs() {
    ptrace(PTRACE_GETREGS, pid, NULL, &regs_struct);
    regs["rax"] = (LL*) &regs_struct.rax;
    regs["rbx"] = (LL*) &regs_struct.rbx;
    regs["rcx"] = (LL*) &regs_struct.rcx;
    regs["rdx"] = (LL*) &regs_struct.rdx;
    regs["rsp"] = (LL*) &regs_struct.rsp;
    regs["rbp"] = (LL*) &regs_struct.rbp;
    regs["rsi"] = (LL*) &regs_struct.rsi;
    regs["rdi"] = (LL*) &regs_struct.rdi;
    regs["rip"] = (LL*) &regs_struct.rip;
    regs["r8"] = (LL*) &regs_struct.r8;
    regs["r9"] = (LL*) &regs_struct.r9;
    regs["r10"] = (LL*) &regs_struct.r10;
    regs["r11"] = (LL*) &regs_struct.r11;
    regs["r12"] = (LL*) &regs_struct.r12;
    regs["r13"] = (LL*) &regs_struct.r13;
    regs["r14"] = (LL*) &regs_struct.r14;
    regs["r15"] = (LL*) &regs_struct.r15;
    regs["flags"] = (LL*) &regs_struct.eflags;
}

void print_reg(const string &name) {
    for (auto &x : reglist) {
        if (x == name) {
            LL val = *regs[name];
            cerr << name << " = " << val
                << " (" << hex << "0x" << val << dec << ")" << endl;
            return;
        }
    }
    cerr << "** '" << name << "' does not exist." << endl;
}

string disone(UC *pos, LL &addr) {
    csh handle;
    cs_insn *insn;
    size_t count;
    string out = "";
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        cerr << "** cs open error." << endl;
        return "";
    }
    count = cs_disasm(handle, pos, MAXASM, addr, 0, &insn);
    if (count > 0) {
        stringstream ss;
        ss << hex << setfill(' ') << setw(12) << insn[0].address << ": "
            << left << setfill(' ') << setw(31) << get_bytes(insn[0].bytes, insn[0].size)
            << left << setfill(' ') << setw(7) << insn[0].mnemonic
            << right << insn[0].op_str << endl << dec;
        addr += insn[0].size;
        out = ss.str();
        cs_free(insn, count);
    }
    else {
        cerr << "** failed to disassemble given code!" << endl;
    }
    cs_close(&handle);
    return out;
}

UC patch_byte(const LL addr, UC c) {
    auto code = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
    ptrace(PTRACE_POKETEXT, pid, addr, (code & 0xffffffffffffff00) | (c & 0xff));
    return code & 0xff;
}

bool isintext(const LL addr) {
    if (state == RUNNING && ispie) {
        LL fixaddr = textbase + texts.addr;
        return fixaddr <= addr && addr <= (fixaddr + texts.size);
    }
    return texts.addr <= addr && addr <= (texts.addr + texts.size);
}

bool chkat(const auto &x, unsigned int at, bool p) {
    if (x.size() > at) return true;
    if (p) cerr << "** missing argument(s)." << endl;
    return false;
}

// don't care:              -1
// hit breakpoint before:   0
// hit breakpoint now:      1
int chkst() {
    int status;
    waitpid(pid, &status, 0);
    if (WIFSTOPPED(status)) {
        if (WSTOPSIG(status) != SIGTRAP) {
            cerr << "** child process " << pid << " stopped by signal (code " << WSTOPSIG(status) << ")" << endl;
            return -1;
        }
        if (hitbp != -1) return 0;
        get_regs();
        // hit the breakpoint or not
        for (auto &x : bpoints) {
            LL tmpaddr = x.addr;
            if (!x.isfix && ispie) tmpaddr += textbase;
            if (tmpaddr == (*regs["rip"]) - 1) {
                hitbp = x.id;
                bpaddr = tmpaddr;
                // show breakpoint message
                dislen = 1;
                LL addrbak = disaddr;
                cerr << "** breakpoint @ ";
                disaddr = tmpaddr;
                disasm();
                disaddr = addrbak;
                dislen = 10;
                // restore the original value
                patch_byte(tmpaddr, x.ori);
                (*regs["rip"])--;
                ptrace(PTRACE_SETREGS, pid, NULL, &regs_struct);
                return 1;
            }
        }
        return -1;
    }
    if (WIFEXITED(status)) {
        if (WIFSIGNALED(status))
            cerr << "** child process " << pid << " terminiated by signal (code " << WTERMSIG(status) << ")" << endl;
        else
            cerr << "** child process " << pid << " terminiated normally (code " << status << ")" << endl;
        pid = 0;
        state = LOADED;
        return -1;
    }
    return -1;
}

void help() {
    cerr << "- break {instruction-address}: add a break point" << endl;
    cerr << "- cont: continue execution" << endl;
    cerr << "- delete {break-point-id}: remove a break point" << endl;
    cerr << "- disasm addr: disassemble instructions in a file or a memory region" << endl;
    cerr << "- dump addr [length]: dump memory content" << endl;
    cerr << "- exit: terminate the debugger" << endl;
    cerr << "- get reg: get a single value from a register" << endl;
    cerr << "- getregs: show registers" << endl;
    cerr << "- help: show this message" << endl;
    cerr << "- list: list break points" << endl;
    cerr << "- load {path/to/a/program}: load a program" << endl;
    cerr << "- run: run the program" << endl;
    cerr << "- vmmap: show memory layout" << endl;
    cerr << "- set reg val: get a single value to a register" << endl;
    cerr << "- si: step into instruction" << endl;
    cerr << "- start: start the program and stop at the first instruction" << endl;
}

void quit() {
    if (eh) {
        elf_close(eh);
        eh = NULL;
    }
    if (code) {
        delete [] code;
        code = NULL;
    }
    if (pid) kill(pid, SIGTERM);
}

void load() {
    if (state != NOLOAD) {
        cerr << "** state must be NOT LOADED." << endl;
        return;
    }
    elf_init();
    if ((eh = elf_open(progpath.c_str())) == NULL) {
        cerr << "** unable to open '" << progpath << "'." << endl;
        return;
    }
    if (elf_load_all(eh) < 0) {
        cerr << "** unable to load '" << progpath << "'." << endl;
        return;
    }
    for (tab = eh->strtab; tab != NULL; tab = tab->next) {
        if (tab->id == eh->shstrndx) break;
    }
    if (tab == NULL) {
        cerr << "** section header string table not found." << endl;
        return;
    }
    for (int i = 0; i < eh->shnum; i++) {
        if (!strcmp(&tab->data[eh->shdr[i].name], ".text")) {
            texts = eh->shdr[i];
            cerr << "** program '" << progpath << "' loaded. " << hex
                << "entry point 0x" << eh->entrypoint
                << ", vaddr 0x" << texts.addr
                << ", offset 0x" << texts.offset
                << ", size 0x" << texts.size << endl << dec;
            state = LOADED;
            break;
        }
    }
    for (int i = 0; i < eh->phnum; i++) {
        LL end = eh->phdr[i].offset + eh->phdr[i].filesz;
        if (eh->phdr[i].offset <= texts.offset && texts.offset <= end) {
            tphdr = eh->phdr[i];
            break;
        }
    }
    if (eh->ehdr.ptr64->e_type == ET_DYN) ispie = true;
}

void vmmap() {
    if (state == LOADED) {
        cerr << hex << setfill('0') << setw(16) << texts.addr << "-"
            << setfill('0') << setw(16) << texts.addr + texts.size << " "
            << flags2rwx(tphdr.flags) << " "
            << setfill('0') << setw(8) << texts.offset << " "
            << progpath << endl << dec;
    }
    else if (state == RUNNING) {
        ifstream f("/proc/" + to_string(pid) + "/maps");
        string s;
        while (getline(f, s)) {
            VS item = split(s);
            VS addr = split(item[0], '-');
            cerr << setfill('0') << setw(16) << addr[0] << "-"
                << setfill('0') << setw(16) << addr[1] << " "
                << item[1].substr(0, 3) << " "
                << item[2];
            if (item.size() > 5) cerr << " " << item[5];
            cerr << endl;
        }
        f.close();
    }
    else {
        cerr << "** state must be LOADED or RUNNING." << endl;
        return;
    }
}

void start() {
    if (state != LOADED) {
        cerr << "** state must be LOADED." << endl;
        return;
    }
    if (pid) {
        cerr << "** program '" << progpath << "' is already running." << endl;
        return;
    }
    pid = fork();
    if (pid < 0) {
        cerr << "** fork error." << endl;
        return;
    }
    else if (pid == 0) {    // child process
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            cerr << "** ptrace error." << endl;
        }
        char **argv = {NULL};
        execvp(progpath.c_str(), argv);
    }
    else {                  // parent process
        int status;
        waitpid(pid, &status, 0);
        // if parent is terminated, kill child
        ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
        // get text base address
        ifstream f("/proc/" + to_string(pid) + "/stat");
        string s;
        getline(f, s);
        VS out = split(s);
        textbase = str2ll(out[25]);
        // set breakpoints
        for (auto &x : bpoints) {
            if (!x.isfix) {
                LL tmpaddr = x.addr;
                if (ispie) tmpaddr += textbase;
                UC tmp = patch_byte(tmpaddr, 0xcc);
                x.ori = tmp;
            }
        }
        cerr << "** pid " << pid << endl;
        state = RUNNING;
    }
}

void cont() {
    if (state != RUNNING) {
        cerr << "** state must be RUNNING." << endl;
        return;
    }
    // restore the breakpoint
    if (hitbp != -1) {
        si();
    }
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    chkst();
}

void run() {
    if (state == RUNNING) {
        cerr << "** program '" << progpath << "' is already running." << endl;
        cont();
    }
    else if (state == LOADED) {
        start();
        cont();
    }
    else {
        cerr << "** state must be LOADED or RUNNING." << endl;
    }
}

void si() {
    if (state != RUNNING) {
        cerr << "** state must be RUNNING." << endl;
        return;
    }
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
    // restore the breakpoint
    if (chkst() == 0 && hitbp != -1) {
        for (auto &x : bpoints) {
            if (x.id == hitbp) {
                UC tmp = patch_byte(bpaddr, 0xcc);
                x.ori = tmp;
                hitbp = -1;
                break;
            }
        }
    }
}

void get(const string &reg) {
    if (state != RUNNING) {
        cerr << "** state must be RUNNING." << endl;
        return;
    }
    get_regs();
    print_reg(reg);
}

void getregs() {
    if (state != RUNNING) {
        cerr << "** state must be RUNNING." << endl;
        return;
    }
    get_regs();
    for (auto &x : reglist) {
        print_reg(x);
    }
}

void set(const string &reg, LL val) {
    if (state != RUNNING) {
        cerr << "** state must be RUNNING." << endl;
        return;
    }
    get_regs();
    *regs[reg] = val;
    ptrace(PTRACE_SETREGS, pid, NULL, &regs_struct);
}

void list() {
    for (auto &x : bpoints) {
        cerr << setfill(' ') << setw(3) << x.id << ":"
            << setfill(' ') << setw(8) << hex << x.addr
            << endl << dec;
    }
}

void bp(const LL addr) {
    if (state == LOADED) {
        if (!isintext(addr)) {
            cerr << "** address must be in the text segment. (LOADED state)" << endl;
            return;
        }
        bpoints.PB({bpid++, addr, 0, false});
    }
    else if (state == RUNNING) {
        UC tmp = patch_byte(addr, 0xcc);
        bpoints.PB({bpid++, addr, tmp, true});
    }
    else {
        cerr << "** state must be LOADED or RUNNING." << endl;
    }
}

void del(int id) {
    bool chk = false;
    for (auto itr = bpoints.begin(); itr != bpoints.end(); itr++) {
        if (id == (*itr).id) {
            patch_byte((*itr).addr, (*itr).ori);
            bpoints.erase(itr);
            chk = true;
            break;
        }
    }
    if (chk) cerr << "** breakpoint " << id << " deleted." << endl;
    else cerr << "** no breakpoint number " << id << "." << endl;
}

void disasm() {
    if (state != LOADED && state != RUNNING) {
        cerr << "** state must be LOADED or RUNNING." << endl;
        return;
    }
    if (disaddr == -1) {
        cerr << "** no addr is given." << endl;
        return;
    }
    if (code == NULL) get_code();
    if (state == LOADED) {
        for (int i = 0; i < dislen; i++) {
            auto pos = (UC*) code + texts.offset + (disaddr - texts.addr);
            LL tmpaddr = disaddr;
            string out = disone(pos, tmpaddr);
            if (out == "" || tmpaddr > texts.addr + texts.size) break;
            else {
                disaddr = tmpaddr;
                cerr << out;
            }
        }
        return;
    }
    if (state == RUNNING) {
        for (int i = 0; i < dislen; i++) {
            if (isintext(disaddr)) {
                LL offset;
                if (ispie) offset = disaddr - textbase;
                else offset = texts.offset + (disaddr - texts.addr);
                auto pos = (UC*) code + offset;
                string out = disone(pos, disaddr);
                cerr << out;
            }
            else {
                string s = get_mem(disaddr);
                auto pos = (UC*) s.c_str();
                string out = disone(pos, disaddr);
                cerr << out;
            }
        }
    }
}

void dump(int sz) {
    if (state != RUNNING) {
        cerr << "** state must be RUNNING." << endl;
        return;
    }
    if (dumpaddr == -1) {
        cerr << "** no addr is given." << endl;
        return;
    }
    int nline = sz / 16, nbytes = sz % 16;
    int n = nline + (nbytes != 0);
    for (int i = 0; i < n; i++) {
        string hexout = "";
        for (int j = 0; j < 2; j++) {
            auto out = ptrace(PTRACE_PEEKTEXT, pid, dumpaddr, NULL);
            hexout += string((char*) &out, 8);
            dumpaddr += 8;
        }
        cerr << hex << setfill(' ') << setw(12) << dumpaddr - 16 << ": "
            << left << setfill(' ') << setw(49) << get_bytes((UC*) hexout.c_str(), 16)
            << right << get_printable(hexout) << endl << dec;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        progpath = argv[1];
        load();
    }
    while (true) {
        cerr << "sdb> ";
        string s;
        getline(cin, s);
        VS line = split(s);
        if (line.empty()) continue;
        string cmd = line[0];
        if (cmd == "break" || cmd == "b") {
            if (chkat(line, 1, true)) bp(str2ll(line[1]));
        }
        else if (cmd == "cont" || cmd == "c") {
            cont();
        }
        else if (cmd == "delete") {
            if (chkat(line, 1, true)) del(stoi(line[1]));
        }
        else if (cmd == "disasm" || cmd == "d") {
            if (chkat(line, 1, false)) disaddr = str2ll(line[1]);
            disasm();
        }
        else if (cmd == "dump" || cmd == "x") {
            if (chkat(line, 1, false)) dumpaddr = str2ll(line[1]);
            if (chkat(line, 2, false)) dump(str2ll(line[2]));
            else dump();
        }
        else if (cmd == "exit" || cmd == "q") {
            quit();
            break;
        }
        else if (cmd == "get" || cmd == "g") {
            if (chkat(line, 1, true)) get(line[1]);
        }
        else if (cmd == "getregs") {
            getregs();
        }
        else if (cmd == "help" || cmd == "h") {
            help();
        }
        else if (cmd == "list" || cmd == "l") {
            list();
        }
        else if (cmd == "load") {
            if (chkat(line, 1, true)) progpath = line[1];
            load();
        }
        else if (cmd == "run" || cmd == "r") {
            run();
        }
        else if (cmd == "vmmap" || cmd == "m") {
            vmmap();
        }
        else if (cmd == "set" || cmd == "s") {
            if (chkat(line, 2, true)) set(line[1], str2ll(line[2]));
        }
        else if (cmd == "si") {
            si();
        }
        else if (cmd == "start") {
            start();
        }
        else {
            cerr << "Undefined command: \"" << cmd << "\".  Try \"help\"." << endl;
        }
    }
    return 0;
}
