#include <unistd.h>

#include <cstring>

#include "session_manager.hpp"

ClientSessionManager::ClientSessionManager() noexcept
{
}

ClientSessionManager::~ClientSessionManager()
{
    clear();
}

bool ClientSessionManager::register_session(socket_t socket, const sockaddr_in& address) noexcept
{
    if (is_socket_registered(socket))
    {
        fprintf(stderr, "cannot register socket; already exists; socket(%d)\n", socket);
        return false;
    }

    {
        std::unique_lock<std::shared_timed_mutex> writer(m_mutex);

        m_sessions.emplace(std::piecewise_construct,
            std::make_tuple(socket),
            std::make_tuple(socket, address));
    }
    return true;
}

bool ClientSessionManager::unregister_session(socket_t socket) noexcept
{
    if (!is_socket_registered(socket))
    {
        return false;
    }

    {
        std::unique_lock<std::shared_timed_mutex> writer(m_mutex);

        m_sessions.erase(socket);
    }
    return true;
}

bool ClientSessionManager::is_socket_registered(socket_t socket) const noexcept
{
    std::shared_lock<std::shared_timed_mutex> reader(m_mutex);

    auto it = m_sessions.find(socket);
    if (it == m_sessions.end())
    {
        return false;
    }

    return true;
}

void ClientSessionManager::clear() noexcept
{
    std::unique_lock<std::shared_timed_mutex> writer(m_mutex);
    m_sessions.clear();
}

sockaddr_in ClientSessionManager::get_sockaddr(socket_t socket) const noexcept
{
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    {
        std::shared_lock<std::shared_timed_mutex> reader(m_mutex);

        auto it = m_sessions.find(socket);
        if (it != m_sessions.end())
        {
            it->second.get_address(&address);
        }
    }

    return address;
}

void ClientSessionManager::for_session(
    socket_t socket, const std::function<void(ClientSession*)>& task) noexcept
{
    std::shared_lock<std::shared_timed_mutex> reader(m_mutex);

    auto it = m_sessions.find(socket);
    if (it == m_sessions.end())
    {
        return;
    }

    task(&(it->second));
}
