#include <cstring>

#include "packet.hpp"

EchoMessagePacket::EchoMessagePacket() noexcept
{
    memset(message, 0, sizeof(message));
}

void EchoMessagePacket::init(const char* msg) noexcept
{
    strncpy(message, msg, sizeof(message));
}

void EchoMessagePacket::serialize(RS::Serializer* ser) noexcept
{
    ser->value(message, sizeof(message));
}
