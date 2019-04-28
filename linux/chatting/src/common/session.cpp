#include <cstdio>
#include <unistd.h>

#include "session.hpp"
#include "network/serialize.hpp"

namespace RS
{

SocketSession::SocketSession(int socket) noexcept
    : m_socket(socket)
{
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

    write(m_socket, packetbuf, serialized_size + PACKET_HEADER_SIZE);
}

}
