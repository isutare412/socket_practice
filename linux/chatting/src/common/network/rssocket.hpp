#ifndef __RSSOCKET_HPP__
#define __RSSOCKET_HPP__

#include <arpa/inet.h>
#include <sys/socket.h>

#include <vector>

namespace RS
{

enum class SocketOption : char
{
    REUSEADDR
};


int
socket();

sockaddr_in
make_sockaddr(
    in_addr_t addr,
    in_port_t port
);

void
setsockopt(
    int socket,
    SocketOption option
);

void
bind(
    int socket,
    const sockaddr_in& addr
);

void
listen(
    int socket,
    int accept_buf_size
);

int
accept(
    int socket,
    const sockaddr_in& addr
);

}

#endif // __RSSOCKET_HPP__
