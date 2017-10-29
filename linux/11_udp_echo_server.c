#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 64

void handle_error(const char* msg);

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_sz;
    int serv_sock;
    int addr_reuse_opt;

    char message[BUF_SIZE];
    int recv_len;

    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        handle_error("UDP socket creation error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (int*)&addr_reuse_opt,
            sizeof(addr_reuse_opt)) == -1)
        handle_error("set socket option error");

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("bind error");

    while (1) {
        clnt_addr_sz = sizeof(clnt_addr);
        recv_len = recvfrom(serv_sock, message, BUF_SIZE, 0,
                (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        sendto(serv_sock, message, recv_len, 0,
                (struct sockaddr*)&clnt_addr, clnt_addr_sz);
    }

    close(serv_sock);
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
