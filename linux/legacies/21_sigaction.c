#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig);

int main(int argc, const char* argv[])
{
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

    alarm(2);
    for (int i = 0; i < 3; ++i) {
        puts("wait...");
        sleep(100);
    }
    return 0;
}

void timeout(int sig)
{
    if (sig != SIGALRM) {
        exit(1);
    }
    puts("Time out!");
    alarm(2);
}
