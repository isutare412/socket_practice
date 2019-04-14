#ifndef __CLIENT_MANAGER_HPP__
#define __CLIENT_MANAGER_HPP__

#include <arpa/inet.h>

#include <map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <functional>

#include "common/network/socket.hpp"

class ClientSocketManager
{
public:
    using socket_t = int;

public:
    ClientSocketManager() noexcept;
    virtual ~ClientSocketManager();

public:
    bool register_socket(socket_t socket, const sockaddr_in& address) noexcept;
    bool unregister_socket(socket_t socket) noexcept;
    bool is_socket_registered(socket_t socket) const noexcept;
    void for_each_socket(std::function<void(socket_t)>&& callback) noexcept;
    void clear() noexcept;
    sockaddr_in get_sockaddr(socket_t socket) const noexcept;

private:
    mutable std::shared_timed_mutex m_mutex;
    std::map<socket_t, sockaddr_in> m_sockets;
};

#endif // __CLIENT_MANAGER_HPP__
