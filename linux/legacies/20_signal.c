#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig);
void keycontrol(int sig);

int main(int argc, const char* argv[])
{
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);

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

void keycontrol(int sig)
{
    if (sig != SIGINT) {
        exit(1);
    }
    puts("CTRL+C pressed");
}
