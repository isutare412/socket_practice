#include "client_session.hpp"

ClientSession::ClientSession(int socket, const sockaddr_in& addr) noexcept
    : RS::SocketSession(socket, addr)
{
}

ClientSession::~ClientSession() noexcept
{
}
