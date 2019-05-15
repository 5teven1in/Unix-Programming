#include <cstdio>
#include <cstdarg>
#include <string>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include "fsmon.hpp"
#include "logger.hpp"
#include "typestring.hpp"

using namespace std;

bool islog = true;
FILE * logfile = NULL;

void output(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(logfile, format, args);
    va_end(args);
}

string readlinkfd(const int &x) {
    char tmp[4096] = {0};
    string s = "/proc/self/fd/" + to_string(x);
    islog = false;
    readlink(s.c_str(), tmp, sizeof(tmp));
    islog = true;
    return tmp;
}

void initlog() {
    if (logfile != NULL) return;
    islog = false;
    char *fname = getenv("MONITOR_OUTPUT");
    if (fname != NULL) logfile = fopen(fname, "w");
    else logfile = stderr;
    islog = true;
}

__attribute__((destructor)) void finlog() {
    islog = false;
    char *fname = getenv("MONITOR_OUTPUT");
    if (fname != NULL) fclose(logfile);
    islog = true;
}

template <class T>
void getfunc(T &old_func, const char* sym) {
    void *handle = dlopen(LIBC, RTLD_LAZY);
    if (handle != NULL) {
        old_func = (T) dlsym(handle, sym);
    }
}

template <class T, class ... TS>
auto hook(T func, const char* sym, TS ... args) {
    static T old_func = NULL;
    getfunc(old_func, sym);
    if (old_func != NULL) {
        return old_func(args...);
    }
}

extern "C" {
    HOOKOP(FD, open, const char *, pathname, int, flags, mode_t, mode);
    HOOKST(int ,__xstat, const char *, pathname, struct stat *, statbuf);
    HOOKST(int ,__lxstat, const char *, pathname, struct stat *, statbuf);
    HOOKVA(int, __isoc99_fscanf, FILE *, stream, const char *, format, vfscanf);
    HOOKVA(int, fprintf, FILE *, stream, const char *, format, vfprintf);
    HOOK1(int, closedir, DIR *, dirp);
    HOOK1(DIR *, opendir, const char *, name);
    HOOK1(struct dirent *, readdir, DIR *, dirp);
    HOOK2(FD, creat, const char *, pathname, mode_t, mode);
    HOOK3(ssize_t, read, FD, fd, void *, buf, size_t, count);
    HOOK3(ssize_t, write, FD, fd, const void *, buf, size_t, count);
    HOOK1(FD, dup, FD, oldfd);
    HOOK2(FD, dup2, FD, oldfd, FD, newfd);
    HOOK1(int, close, FD, fd);
    HOOK4(ssize_t, pwrite, FD, fd, const void *, buf, size_t, count, off_t, offset);
    HOOK2(FILE *, fopen, const char *, pathname, const char *, mode);
    HOOK1(int, fclose, FILE *, stream);
    HOOK4(size_t, fread, void *, ptr, size_t, size, size_t, nmemb, FILE *, stream);
    HOOK4(size_t, fwrite, const void *, ptr, size_t, size, size_t, nmemb, FILE *, stream);
    HOOK1(int, fgetc, FILE *, stream);
    HOOK3(char *, fgets, char *, s, int, size, FILE *, stream);
    HOOK1(int, chdir, const char *, path);
    HOOK3(int, chown, const char *, pathname, uid_t, owner, gid_t, group);
    HOOK2(int, chmod, const char *, pathname, mode_t, mode);
    HOOK1(int, remove, const char *, pathname);
    HOOK2(int, rename, const char *, oldpath, const char *, newpath);
    HOOK2(int, link, const char *, oldpath, const char *, newpath);
    HOOK1(int, unlink, const char *, pathname);
    HOOK3(ssize_t, readlink, const char *, pathname, char *, buf, size_t, bufsiz);
    HOOK2(int, symlink, const char *, target, const char *, linkpath);
    HOOK2(int, mkdir, const char *, pathname, mode_t, mode);
    HOOK1(int, rmdir, const char *, pathname);
}
