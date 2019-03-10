#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "common/network/rssocket.hpp"
#include "common/network/tools.hpp"

static constexpr int LISTEN_QUEUE_SIZE = 30;

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_sock = RS::socket();
    printf("socket created; socket_fd(%d)\n", server_sock);

    sockaddr_in sock_addr = RS::make_sockaddr(
        htonl(INADDR_ANY), htons(atoi(argv[1]))
    );

    RS::setsockopt(server_sock, RS::SocketOption::REUSEADDR);
    RS::bind(server_sock, sock_addr);
    printf("bind address to socket; socket_fd(%d) address(%s)\n",
        server_sock, RS::sockaddr_to_string(sock_addr).c_str());

    RS::listen(server_sock, LISTEN_QUEUE_SIZE);
    printf("start to listen; socket_fd(%d) queue_size(%d)\n",
        server_sock, LISTEN_QUEUE_SIZE);

    while (true)
    {
        sockaddr_in clnt_addr;
        int clnt_sock = RS::accept(server_sock, clnt_addr);
        printf("client connected; address(%s)\n",
            RS::sockaddr_to_string(clnt_addr).c_str());

        close(clnt_sock);
    }

    return EXIT_SUCCESS;
}