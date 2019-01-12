#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "rssocklib.h"

int main(int argc, const char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        handle_error("socket()");
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("connect()");
    }

    int pipe_fds[2];
    pipe(pipe_fds);

    pid_t pid = fork();
    if (pid == 0) { // child
        close(sock);            // close server socket
        close(pipe_fds[0]);     // close read end of pipe

        // pipe stdout to file input
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
            perror("dup2()");
            return EXIT_FAILURE;
        }

        execl("/bin/ls", "ls", "-l", (char*)NULL);
        return EXIT_SUCCESS;
    }

    close(pipe_fds[1]); // close write end of pipe

    char buf[BUFSIZ];
    int read_len;
    while ((read_len = read(pipe_fds[0], buf, BUFSIZ)) != 0) {
        write(sock, buf, read_len);
    }

    close(sock);
    return EXIT_SUCCESS;
}
