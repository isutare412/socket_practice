#ifndef __CLIENT_MANAGER_HPP__
#define __CLIENT_MANAGER_HPP__

#include <arpa/inet.h>

#include <map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <functional>

#include "common/network/socket.hpp"
#include "common/session.hpp"

class ClientSessionManager
{
public:
    using socket_t = int;

public:
    ClientSessionManager() noexcept;
    virtual ~ClientSessionManager();

public:
    bool register_session(socket_t socket, const sockaddr_in& address) noexcept;
    bool unregister_session(socket_t socket) noexcept;
    bool is_socket_registered(socket_t socket) const noexcept;
    void clear() noexcept;
    sockaddr_in get_sockaddr(socket_t socket) const noexcept;

private:
    mutable std::shared_timed_mutex m_mutex;
    std::map<socket_t, RS::SocketSession> m_sessions;
};

#endif // __CLIENT_MANAGER_HPP__
