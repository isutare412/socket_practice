#ifndef __CLIENT_HANDLER_HPP__
#define __CLIENT_HANDLER_HPP__

#include "common/network/packet_handler.hpp"
#include "common/network/packet_base.hpp"

class ClientHandler
    : public RS::PacketHandler<RS::PacketBase, (int32_t)RS::PacketType::PACKET_MAX>
{
public:
    ClientHandler() noexcept;
    virtual ~ClientHandler() noexcept {};
};

#endif // __CLIENT_HANDLER_HPP__