#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    int tcp_sock, udp_sock;
    tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    printf("SOCK_STREAM: %d\n", SOCK_STREAM);
    printf("SOCK_DGRAM: %d\n", SOCK_DGRAM);

    int sock_type;
    socklen_t optlen;
    optlen = sizeof(sock_type);

    if (getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen) != 0) {
        handle_error("getsockopt()");
    }
    printf("Socket type of SOCK_STREAM: %d\n", sock_type);

    if (getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen) != 0) {
        handle_error("getsockopt()");
    }
    printf("Socket type of SOCK_DGRAM: %d\n", sock_type);

    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
