#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common/network/socket.hpp"
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

    char message[256] = { '\0', };
    while (true)
    {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\r\n")] = '\0';

        if (!strcmp(message, "q") || !strcmp(message, "Q"))
        {
            break;
        }

        int write_len = write(sock, message, strlen(message));
        memset(message, 0, sizeof(message));

        RS::ErrorType error = RS::read_nbytes(sock, message, write_len);
        if (error == RS::ErrorType::SYS_ERROR)
        {
            fprintf(stderr, "failed to read from socket\n");
            close(sock);
            exit(EXIT_FAILURE);
        }

        printf("Message from server: %s\n", message);
    }

    close(sock);

    return EXIT_SUCCESS;
}