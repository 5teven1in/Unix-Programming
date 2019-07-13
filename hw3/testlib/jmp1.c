#include <setjmp.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

typedef void (*proc_t)();
static jmp_buf jb;

#define FUNBODY(m, from) { write(1, m, strlen(m)); longjmp(jb, from); }

void a() FUNBODY("This is function a().\n", 0);
void b() FUNBODY("This is function b().\n", 2);
void c() FUNBODY("This is function c().\n", 3);
void d() FUNBODY("This is function d().\n", 4);
void e() FUNBODY("This is function e().\n", 5);
void f() FUNBODY("This is function f().\n", 6);
void g() FUNBODY("This is function g().\n", 7);
void h() FUNBODY("This is function h().\n", 8);
void i() FUNBODY("This is function i().\n", 9);
void j() FUNBODY("This is function j().\n", 10);

proc_t funs[] = { a, b, c, d, e, f, g, h, i, j };

int main() {
    volatile int i = 0;
    if(setjmp(jb) != 0) {
            i++;
        }
    if(i < 10) funs[i]();
    return 0;
}
