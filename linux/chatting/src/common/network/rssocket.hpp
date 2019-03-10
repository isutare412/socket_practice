#ifndef __RSSOCKET_HPP__
#define __RSSOCKET_HPP__

#include <arpa/inet.h>
#include <sys/socket.h>

namespace RS
{

enum class SocketOption : char
{
    REUSEADDR
};


int
socket() noexcept;

sockaddr_in
make_sockaddr(
    in_addr_t addr,
    in_port_t port
) noexcept;

void
setsockopt(
    int socket,
    SocketOption option
) noexcept;

void
bind(
    int socket,
    const sockaddr_in& addr
) noexcept;

void
listen(
    int socket,
    int accept_buf_size
) noexcept;

int
accept(
    int socket,
    const sockaddr_in& addr
) noexcept;

}

#endif // __RSSOCKET_HPP__
