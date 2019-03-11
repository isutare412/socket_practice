#ifndef __CHAT_SERVER_HPP__
#define __CHAT_SERVER_HPP__

#include <arpa/inet.h>

#include "common/thread/thread_pool.hpp"
#include "client_socket_manager.hpp"

class ChatServer
{
public:
    ChatServer() noexcept;
    virtual ~ChatServer();

public:
    bool
    initialize(
        in_port_t port,
        uint32_t listen_queue_size
    ) noexcept;

    bool
    run() noexcept;

private:
    void
    close_client(
        int socket
    ) noexcept;

private:
    void
    serv_client(
        int sock_fd
    ) noexcept;

    void
    send_file(
        int sock_fd,
        const char* filename
    ) noexcept;

private:
    int m_server_socket;
    sockaddr_in m_server_addr;

    RS::ThreadPool m_threads;
    ClientSocketManager m_socket_manager;
};

#endif // __CHAT_SERVER_HPP__
