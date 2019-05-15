#pragma once

#include <string>

using namespace std;

typedef int FD;

#define LIBC "libc.so.6"

#define __OPEN_NEEDS_MODE(oflag) (((oflag) & 0100) != 0)

#define FNclosedir "closedir"
#define FNopendir "opendir"
#define FNreaddir "readdir"
#define FNcreat "creat"
#define FNopen "open"
#define FNread "read"
#define FNwrite "write"
#define FNdup "dup"
#define FNdup2 "dup2"
#define FNclose "close"
#define FN__lxstat "lstat"
#define FN__xstat "stat"
#define FNpwrite "pwrite"
#define FNfopen "fopen"
#define FNfclose "fclose"
#define FNfread "fread"
#define FNfwrite "fwrite"
#define FNfgetc "fgetc"
#define FNfgets "fgets"
#define FN__isoc99_fscanf "fscanf"
#define FNfprintf "fprintf"
#define FNchdir "chdir"
#define FNchown "chown"
#define FNchmod "chmod"
#define FNremove "remove"
#define FNrename "rename"
#define FNlink "link"
#define FNunlink "unlink"
#define FNreadlink "readlink"
#define FNsymlink "symlink"
#define FNmkdir "mkdir"
#define FNrmdir "rmdir"

#define HOOK1(ret, sym, t1, p1)                                                                             \
    ret sym(t1 p1) {                                                                                        \
        ret tmp;                                                                                            \
        if (islog) {                                                                                        \
            initlog();                                                                                      \
            Logger l(FN##sym, false);                                                                       \
            if (FN##sym == FNclose || FN##sym == FNfclose || FN##sym == FNclosedir || FN##sym == FNdup) {   \
                l = l.add(#t1, p1);                                                                         \
                tmp = hook(sym, #sym, p1);                                                                  \
                l.addret(#ret, tmp).log();                                                                  \
            }                                                                                               \
            else {                                                                                          \
                tmp = hook(sym, #sym, p1);                                                                  \
                l.add(#t1, p1).addret(#ret, tmp).log();                                                     \
            }                                                                                               \
        }                                                                                                   \
        return tmp;                                                                                         \
    }

#define HOOK2(ret, sym, t1, p1, t2, p2)                                 \
    ret sym(t1 p1, t2 p2) {                                             \
        ret tmp;                                                        \
        if (islog) {                                                    \
            initlog();                                                  \
            Logger l(FN##sym, false);                                   \
            if (FN##sym == FNdup2) {                                    \
                l = l.add(#t1, p1);                                     \
                tmp = hook(sym, #sym, p1, p2);                          \
                l.add(#t2, p2).addret(#ret, tmp).log();                 \
            }                                                           \
            else {                                                      \
                tmp = hook(sym, #sym, p1, p2);                          \
                l.add(#t1, p1).add(#t2, p2).addret(#ret, tmp).log();    \
            }                                                           \
        }                                                               \
        return tmp;                                                     \
    }

#define HOOK3(ret, sym, t1, p1, t2, p2, t3, p3)                                 \
    ret sym(t1 p1, t2 p2, t3 p3) {                                              \
        ret tmp = hook(sym, #sym, p1, p2, p3);                                  \
        if (islog) {                                                            \
            initlog();                                                          \
            Logger l(FN##sym, false);                                           \
            l.add(#t1, p1).add(#t2, p2).add(#t3, p3).addret(#ret, tmp).log();   \
        }                                                                       \
        return tmp;                                                             \
    }

#define HOOK4(ret, sym, t1, p1, t2, p2, t3, p3, t4, p4)                                     \
    ret sym(t1 p1, t2 p2, t3 p3, t4 p4) {                                                   \
        ret tmp = hook(sym, #sym, p1, p2, p3, p4);                                          \
        if (islog) {                                                                        \
            initlog();                                                                      \
            Logger l(FN##sym, false);                                                       \
            l.add(#t1, p1).add(#t2, p2).add(#t3, p3).add(#t4, p4).addret(#ret, tmp).log();  \
        }                                                                                   \
        return tmp;                                                                         \
    }

#define HOOKVA(ret, sym, t1, p1, t2, p2, func)                      \
    ret sym(t1 p1, t2 p2, ...) {                                    \
        va_list args;                                               \
        va_start(args, p2);                                         \
        ret tmp = func(p1, p2, args);                               \
        if (islog) {                                                \
            initlog();                                              \
            Logger l(FN##sym, true);                                \
            l.add(#t1, p1).add(#t2, p2).addret(#ret, tmp).log();    \
        }                                                           \
        va_end(args);                                               \
        return tmp;                                                 \
    }

#define HOOKST(ret, sym, t1, p1, t2, p2)                            \
    ret sym(int ver, t1 p1, t2 p2) {                                \
        ret tmp = hook(sym, #sym, 1, p1, p2);                       \
        if (islog) {                                                \
            initlog();                                              \
            Logger l(FN##sym, false);                               \
            l.add(#t1, p1).add(#t2, p2).addret(#ret, tmp).log();    \
        }                                                           \
        return tmp;                                                 \
    }

#define HOOKOP(ret, sym, t1, p1, t2, p2, t3, p3)            \
    ret sym(t1 p1, t2 p2, t3 p3 = 04) {                     \
        ret tmp = hook(sym, #sym, p1, p2, p3);              \
        if (islog) {                                        \
            initlog();                                      \
            Logger l(FN##sym, false);                       \
            l = l.add(#t1, p1).add(#t2, p2);                \
            if (__OPEN_NEEDS_MODE(p2)) l = l.add(#t3, p3);  \
            l.addret(#ret, tmp).log();                      \
        }                                                   \
        return tmp;                                         \
    }

void output(const char *format, ...);
string readlinkfd(const int &x);
