#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void handler_sigchld(int sig);

int main(int argc, const char* argv[])
{
    struct sigaction sa;
    sa.sa_handler = handler_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    pid_t pid = fork();
    if (pid == 0) {
        puts("Hello from child!");
        puts("Child gonna sleep 5 seconds...");
        sleep(5);
        return 7;
    }

    puts("Hello from parent!");
    puts("Parent gonna sleep 100 seconds...");
    sleep(100);
    return 0;
}

void handler_sigchld(int sig)
{
    if (sig != SIGCHLD) {
        exit(1);
    }

    int exit_status;
    pid_t chldpid = waitpid(-1, &exit_status, WNOHANG);
    if (!WIFEXITED(exit_status)) {
        puts("Child exited abnormally!!");
        exit(1);
    }

    int exitno = WEXITSTATUS(exit_status);
    printf("Child(%d) exited with exit(%d)\n", chldpid, exitno);
}
