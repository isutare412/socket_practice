#ifndef __CLIENT_SESSION_HPP__
#define __CLIENT_SESSION_HPP__

#include "common/session.hpp"

class ClientSession : public RS::SocketSession
{
public:
    ClientSession(int socket, const sockaddr_in& addr) noexcept;
    ClientSession(const ClientSession&) = delete;
    virtual ~ClientSession() noexcept;

    ClientSession& operator=(const ClientSession&) = delete;
};

#endif // __CLIENT_SESSION_HPP__