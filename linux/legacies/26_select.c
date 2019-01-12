#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#define BUF_SIZE 32

void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    fd_set reads, temps;
    FD_ZERO(&reads);
    FD_SET(STDIN_FILENO, &reads);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    while (1) {
        temps = reads;

        int result = select(1, &temps, 0, 0, &timeout);
        if (result == -1) {
            handle_error("select()");
        }

        if (result == 0) {
            puts("Time-out!");
            continue;
        }

        if (FD_ISSET(STDIN_FILENO, &temps)) {
            char buf[BUF_SIZE];
            int str_len = read(STDIN_FILENO, buf, BUF_SIZE - 1);
            buf[str_len] = '\0';
            printf("message from console: %s", buf);
        }
    }
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
