#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "rssocklib.h"

int rsserv_sockbind(short port)
{
    // make socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        handle_error("socket()");
    }

    // set socket address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    // set socket option that reuses time-waiting port
    int addr_reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &addr_reuse, sizeof(addr_reuse)) == -1) {
        handle_error("setsockopt()");
    }

    if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("bind()");
    }

    return sock;
}

int rsserv_acceptlog(int sock)
{
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int clnt_sock;

    // accept from sock
    clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
    if (clnt_sock == -1) {
        handle_error("accept()");
    }

    // log client connection
    printf("client connected; ip(%s), port(%d), fd(%d) \n", inet_ntoa(clnt_addr.sin_addr),
            ntohs(clnt_addr.sin_port), clnt_sock);

    return clnt_sock;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
