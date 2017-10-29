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
    struct sockaddr_in serv_addr, from_addr;
    socklen_t from_addr_sz;
    int sock;

    char message[BUF_SIZE];
    int recv_len;

    if (argc != 3) {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        handle_error("UDP socket creation error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    while (1) {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        sendto(sock, message, strlen(message), 0,
                (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        from_addr_sz = sizeof(from_addr);
        recv_len = recvfrom(sock, message, BUF_SIZE, 0,
                (struct sockaddr*)&from_addr, &from_addr_sz);
        message[recv_len] = '\0';
        printf("Message from server: %s", message);
    }

    close(sock);
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
