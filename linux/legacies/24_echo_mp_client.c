#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void read_routine(int sock);
void write_routine(int sock);
void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // make socket to connect
    int sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        handle_error("socket()");
    }

    // set socket address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("connect()");
    }

    // create child process for write routine
    pid_t pid = fork();
    if (pid == -1) {
        close(sock);
        handle_error("fork()");
    }

    if (pid == 0) { // child process
        write_routine(sock);
        close(sock);
        return 0;
    }

    read_routine(sock);
    close(sock);
    return 0;
}

void read_routine(int sock)
{
    char buf[BUF_SIZE];
    while (1) {
        int str_len = read(sock, buf, BUF_SIZE - 1);
        if (str_len == 0) {
            return;
        }

        buf[str_len] = '\0';
        printf("Message from server: %s", buf);
    }
}

void write_routine(int sock)
{
    char buf[BUF_SIZE];
    while (1) {
        fgets(buf, BUF_SIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }

        write(sock, buf, strlen(buf));
    }
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
