#include <unistd.h>

#include <cstring>

#include "socket_manager.hpp"

ClientSocketManager::ClientSocketManager() noexcept
{
}

ClientSocketManager::~ClientSocketManager()
{
    clear();
}

bool
ClientSocketManager::register_socket(
    socket_t socket,
    const sockaddr_in& address
) noexcept
{
    if (is_socket_registered(socket))
    {
        fprintf(stderr, "cannot register socket; already exists; socket(%d)\n", socket);
        return false;
    }

    {
        std::unique_lock<std::shared_timed_mutex> writer(m_mutex);

        m_sockets[socket] = address;
    }
    return true;
}

bool
ClientSocketManager::unregister_socket(
    socket_t socket
) noexcept
{
    if (!is_socket_registered(socket))
    {
        return false;
    }

    {
        std::unique_lock<std::shared_timed_mutex> writer(m_mutex);

        m_sockets.erase(socket);
    }
    return true;
}

bool
ClientSocketManager::is_socket_registered(
    socket_t socket
) const noexcept
{
    std::shared_lock<std::shared_timed_mutex> reader(m_mutex);

    auto it = m_sockets.find(socket);
    if (it == m_sockets.end())
    {
        return false;
    }

    return true;
}

void
ClientSocketManager::for_each_socket(
    std::function<void(socket_t)>&& callback
) noexcept
{
    std::shared_lock<std::shared_timed_mutex> reader(m_mutex);

    for (const auto& socket_pair : m_sockets)
    {
        callback(socket_pair.first);
    }
}

void
ClientSocketManager::clear() noexcept
{
    std::unique_lock<std::shared_timed_mutex> writer(m_mutex);
    m_sockets.clear();
}

sockaddr_in
ClientSocketManager::get_sockaddr(
    socket_t socket
) const noexcept
{
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    {
        std::shared_lock<std::shared_timed_mutex> reader(m_mutex);

        auto it = m_sockets.find(socket);
        if (it != m_sockets.end())
        {
            address = it->second;
        }
    }

    return address;
}
