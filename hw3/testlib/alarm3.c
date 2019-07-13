#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void handler(int s) { /* do nothing */ }

int main() {
    sigset_t s;
    printf("%ld\n", sizeof(s));
    sigemptyset(&s);
    sigaddset(&s, SIGALRM);
    sigprocmask(SIG_BLOCK, &s, NULL);
    signal(SIGALRM, SIG_IGN);
    signal(SIGINT, handler);
    alarm(1);
    pause();
    if(sigpending(&s) < 0) perror("sigpending");
    if(sigismember(&s, SIGALRM)) {
            char m[] = "sigalrm is pending.\n";
            write(1, m, sizeof(m));
        } else {
                char m[] = "sigalrm is not pending.\n";
                write(1, m, sizeof(m));
            }
    return 0;
}
