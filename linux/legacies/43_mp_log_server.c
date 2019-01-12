#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "rssocklib.h"

#define LOG_NAME_SIZE 64

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int serv_sock = rsserv_sockbind(atoi(argv[1]));
    if (listen(serv_sock, LISTEN_QUEUE_SIZE) == -1) {
        handle_error("listen()");
    }

    int log_num = 0;
    while (1) {
        int clnt_sock = rsserv_acceptlog(serv_sock);
        ++log_num;

        pid_t pid = fork();
        if (pid > 0) { // parent process
            close(clnt_sock);
            continue;
        }

        // child process
        close(serv_sock);

        char log_name[LOG_NAME_SIZE];
        snprintf(log_name, LOG_NAME_SIZE, "log_%02d.txt", log_num);
        FILE* log = fopen(log_name, "w");

        char buf[BUFSIZ];
        int read_len;
        while ((read_len = read(clnt_sock, buf, BUFSIZ)) != 0) {
            fwrite(buf, sizeof(buf[0]), read_len, log);
        }

        printf("log done; log(%s)\n", log_name);

        fclose(log);
        close(clnt_sock);

        return 0;
    }
    close(serv_sock);

    return EXIT_SUCCESS;
}
