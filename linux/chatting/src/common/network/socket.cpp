#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <iostream>

#include "socket.hpp"
#include "tools.hpp"

namespace RS
{

int socket() noexcept
{
    // open TCP socket
    int sock = ::socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        handle_perror("TCP socket creation error");
    }

    return sock;
}

sockaddr_in make_sockaddr(in_addr_t addr, in_port_t port) noexcept
{
    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = addr;
    serv_addr.sin_port = port;

    return serv_addr;
}

void setsockopt(int socket, SocketOption option) noexcept
{
    switch (option)
    {
        case SocketOption::REUSEADDR:
        {
            int addr_reuse = 1;
            if (::setsockopt(
                socket, SOL_SOCKET, SO_REUSEADDR,
                (int*)&addr_reuse, sizeof(addr_reuse)) == -1)
            {
                handle_perror(
                    "set socket option error; SocketOption(%hhd)", option);
            }
            break;
        }
        default:
        {
            std::cerr << "unhandled SocketOption\n";
            break;
        }
    }
}

void bind(int socket, const sockaddr_in& addr) noexcept
{
    if (::bind(socket,
        (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        handle_perror("failed to bind; address(%s)",
            sockaddr_to_string(addr).c_str());
    }
}

void listen(int socket, int accept_buf_size) noexcept
{
    if (::listen(socket, accept_buf_size) == -1)
    {
        handle_perror("failed to listen socket; socket(%d); buf_size(%d)",
            socket, accept_buf_size);
    }
}

int accept(int socket, sockaddr_in& addr) noexcept
{
    socklen_t addr_sz = sizeof(addr);
    int clnt_sock = ::accept(
        socket, (struct sockaddr*)&addr, &addr_sz);
    if (clnt_sock == -1)
    {
        handle_perror("failed to accept; address(%s)",
            sockaddr_to_string(addr).c_str());
    }

    return clnt_sock;
}

void connect(int socket, const sockaddr_in& addr) noexcept
{
    if (::connect(socket, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        handle_perror("failed to connect; address(%s)", sockaddr_to_string(addr).c_str());
    }
}

}
