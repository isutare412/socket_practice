#ifndef __SESSION_HPP__
#define __SESSION_HPP__

#include <arpa/inet.h>

#include <shared_mutex>

#include "network/packet_base.hpp"

namespace RS
{

class SocketSession
{
public:
    SocketSession(int socket, const sockaddr_in& addr) noexcept;
    SocketSession(const SocketSession&) = delete;
    virtual ~SocketSession() noexcept;

    SocketSession& operator=(const SocketSession&) = delete;

public:
    void send_packet(PacketBase* packet) noexcept;
    void get_address(sockaddr_in* addr) const noexcept;

private:
    mutable std::shared_timed_mutex m_mutex;
    int m_socket;
    sockaddr_in m_address;
};

} // RS

#endif // __SESSION_HPP__