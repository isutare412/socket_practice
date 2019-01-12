#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char* argv[])
{
    pid_t pid = fork();
    if (pid == 0) { // child process
        sleep(5);
        return 24;
    }
    
    int status;
    while (!waitpid(-1, &status, WNOHANG)) {
        sleep(1);
        puts("sleep 1 second...");
    }

    if (!WIFEXITED(status)) {
        fprintf(stderr, "Child(%d) got signal to stop\n", pid);
        exit(1);
    }

    printf("Child exited normally with exit_code(%d)\n", WEXITSTATUS(status));
    return 0;
}
