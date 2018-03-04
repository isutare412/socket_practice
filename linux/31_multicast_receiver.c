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
    if (argc != 3) {
        printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }

    // make socket
    int recv_sock;
    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);

    // set socket address
    struct sockaddr_in mul_addr;
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    mul_addr.sin_port = htons(atoi(argv[2]));

    // bind socket for receiving
    if (bind(recv_sock, (struct sockaddr*)&mul_addr, sizeof(mul_addr)) == -1) {
        handle_error("bind()");
    }

    // set multicast group address
    struct ip_mreq join_addr;
    join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

    // join to multicast group
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_addr, sizeof(join_addr));

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
