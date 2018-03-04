#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 128

void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <PORT>\n", argv[0]);
        exit(1);
    }

    // make socket
    int recv_sock;
    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);

    // set socket address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    // bind socket for receiving
    if (bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        handle_error("bind()");
    }

    while(1) {
        char buf[BUF_SIZE];
        int str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len < 0) {
            break;
        }

        buf[str_len] = '\0';
        fputs(buf, stdout);
    }
    close(recv_sock);

    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
