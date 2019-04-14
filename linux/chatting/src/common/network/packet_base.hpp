#ifndef __PACKET_HPP__
#define __PACKET_HPP__

#include "serialize.hpp"

namespace RS
{

enum class PacketType
{
    ECHO_MESSAGE,

    PACKET_MAX
};

struct PacketBase : public Serializable
{
};

} // RS

#endif // __PACKET_HPP__
