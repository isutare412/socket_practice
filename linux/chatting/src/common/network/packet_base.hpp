#ifndef __PACKET_HPP__
#define __PACKET_HPP__

#include "serialize.hpp"

namespace RS
{

#define PACKET_HEADER_SIZE (sizeof(int32_t) * 2)
#define PACKET_MAX_SIZE (PACKET_HEADER_SIZE + SERIALIZE_BUF_SIZE)

enum class PacketType
{
    ECHO_MESSAGE,

    PACKET_MAX
};

struct PacketBase : public Serializable
{
    PacketType type;

    PacketBase(PacketType type) : type(type) {}
};

} // RS

#endif // __PACKET_HPP__
