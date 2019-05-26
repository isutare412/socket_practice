#include <cstdio>

#include "client_handler.hpp"
#include "common/packet.hpp"
#include "common/network/tools.hpp"

static bool on_echo_message(ClientSession* session, const RS::EchoMessagePacket& packet)
{
    printf("Got message: %s\n", packet.message);

    sockaddr_in addr;
    session->get_address(&addr);
    printf("Address: %s\n", RS::sockaddr_to_string(addr).c_str());

    return true;
}

ClientHandler::ClientHandler() noexcept
{
    register_handler(on_echo_message);
}
