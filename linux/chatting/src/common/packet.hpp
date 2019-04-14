#include "network/packet_base.hpp"

namespace RS
{

struct EchoMessagePacket : public RS::PacketBase
{
    char message[256];

    EchoMessagePacket() noexcept;
    void init(const char* msg) noexcept;
    void serialize(RS::Serializer* ser) noexcept override;

    static RS::PacketType get_type() noexcept
    {
        return RS::PacketType::ECHO_MESSAGE;
    }
};

} // RS
