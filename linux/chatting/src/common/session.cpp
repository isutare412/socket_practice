#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <sys/errno.h>

#include "session.hpp"
#include "network/serialize.hpp"

namespace RS
{

SocketSession::SocketSession(int socket, const sockaddr_in& addr) noexcept
    : m_socket(socket)
{
    ::memcpy(&m_address, &addr, sizeof(m_address));
}

SocketSession::~SocketSession() noexcept
{
    int close_result = ::close(m_socket);
    if (close_result == -1)
    {
        if (errno == EBADF)
        {
            printf("already closed socket; fd(%d)\n", m_socket);
        }
        else
        {
            fprintf(stderr, "failed to close socket; fd(%d)\n", m_socket);
        }

        return;
    }

    printf("successfully closed socket; fd(%d)\n", m_socket);
}

void SocketSession::send_packet(PacketBase* packet) noexcept
{
    RS::OSerializer ser;
    packet->serialize(&ser);

    char packetbuf[PACKET_MAX_SIZE];
    int32_t* const packet_type_pos = reinterpret_cast<int32_t*>(packetbuf);
    int32_t* const buffer_size_pos = reinterpret_cast<int32_t*>(packetbuf + sizeof(int32_t));
    char* const serialize_pos = packetbuf + PACKET_HEADER_SIZE;

    int32_t serialized_size = ser.get(serialize_pos, SERIALIZE_BUF_SIZE);
    if (serialized_size < 0)
    {
        fprintf(stderr, "cannot serialize; packet is too large\n");
        return;
    }

    *packet_type_pos = htonl((int32_t)packet->type);
    *buffer_size_pos = htonl(serialized_size);

    {
        std::unique_lock<std::shared_timed_mutex> writer(m_mutex);

        ::write(m_socket, packetbuf, serialized_size + PACKET_HEADER_SIZE);
    }
}

void SocketSession::get_address(sockaddr_in* addr) const noexcept
{
    std::shared_lock<std::shared_timed_mutex> reader(m_mutex);

    ::memcpy(addr, &m_address, sizeof(decltype(*addr)));
}

}
