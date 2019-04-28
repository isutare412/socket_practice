#ifndef __SESSION_HPP__
#define __SESSION_HPP__

#include "network/packet_base.hpp"

namespace RS
{

class SocketSession
{
public:
    SocketSession(int socket) noexcept;
    SocketSession(const SocketSession&) = delete;

    SocketSession& operator=(const SocketSession&) = delete;

public:
    void send_packet(PacketBase* packet) noexcept;

private:
    int m_socket;
};

} // RS

#endif // __SESSION_HPP__