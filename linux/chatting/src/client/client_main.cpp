#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common/network/rssocket.hpp"
#include "common/network/tools.hpp"

int main(int argc, const char* argv[])
{
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock = RS::socket();
    printf("socket created; socket_fd(%d)\n", sock);

    sockaddr_in sock_addr = RS::make_sockaddr(
        inet_addr(argv[1]), htons(atoi(argv[2]))
    );

    RS::setsockopt(sock, RS::SocketOption::REUSEADDR);
    RS::connect(sock, sock_addr);
    printf("connected to server; address(%s)\n",
        RS::sockaddr_to_string(sock_addr).c_str());

    char read_buf[256] = { '\0', };
    RS::ErrorType error = RS::read_nbytes(sock, read_buf, sizeof(read_buf) - 1);
    if (error != RS::ErrorType::NO_ERROR
        && error != RS::ErrorType::EOF_DETECTED)
    {
        fprintf(stderr, "failed to read from socket\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Got file from server...\n");
    printf("%s\n", read_buf);

    close(sock);

    return EXIT_SUCCESS;
}