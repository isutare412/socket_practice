#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ADDR_PORT_LEN 32
#define BUF_SIZE 1024

void error_handling(const char* msg);

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_len;

    int serv_sock, clnt_sock;
    int addr_reuse = 1;
    int str_len;

    char clnt_addr_buf[ADDR_PORT_LEN];
    char message[BUF_SIZE];

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1)
        error_handling("socket()");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&addr_reuse,
            sizeof(addr_reuse)) == -1)
        error_handling("setsockopt()");

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind()");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen()");

    clnt_addr_len = sizeof(clnt_addr);

    while (1) {
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1)
            error_handling("accept()");

        snprintf(clnt_addr_buf, sizeof(clnt_addr_buf), "%s:%d",
                inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);
        printf("client connected: %s\n", clnt_addr_buf);

        while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0) {
            write(clnt_sock, message, str_len);
        }

        close(clnt_sock);
    }

    close(serv_sock);
    return 0;
}

void error_handling(const char* msg)
{
    perror(msg);
    exit(1);
}
