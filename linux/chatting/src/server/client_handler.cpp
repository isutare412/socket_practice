#include <cstdio>

#include "client_handler.hpp"
#include "common/packet.hpp"

static bool on_echo_message(const RS::EchoMessagePacket& packet)
{
    printf("Got message: %s\n", packet.message);
    return true;
}

ClientHandler::ClientHandler() noexcept
{
    register_handler(on_echo_message);
}
