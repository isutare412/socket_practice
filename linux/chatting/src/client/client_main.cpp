#include <cstdio>
#include <cstdlib>
#include <unistd.h>
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

    char read_buf[256];
    int cur_buf_cursor = 0;
    int remain_buf_size = sizeof(read_buf);
    int read_cnt;
    while (
        (read_cnt = read(sock,
                         &read_buf[cur_buf_cursor],
                         remain_buf_size - 1)
        ) > 0
    )
    {
        cur_buf_cursor += read_cnt;
        remain_buf_size -= read_cnt;
    }

    if (read_cnt == -1)
    {
        RS::handle_perror("read error");
    }

    printf("Got file from server...\n");
    printf("%s\n", read_buf);

    close(sock);

    return EXIT_SUCCESS;
}