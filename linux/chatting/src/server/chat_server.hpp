#ifndef __CHAT_SERVER_HPP__
#define __CHAT_SERVER_HPP__

#include <arpa/inet.h>

#include <mutex>

#include "common/network/poll.hpp"
#include "common/thread/thread_pool.hpp"
#include "client_handler.hpp"
#include "socket_manager.hpp"

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
    bool 
    handle_client(
        int socket
    ) noexcept;

    void
    accept_client(
        int socket
    ) noexcept;

    void
    close_client(
        int socket
    ) noexcept;

    void
    register_socket(
        int socket,
        const sockaddr_in& addr
    ) noexcept;

private:
    void
    echo_client(
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
    ClientHandler m_client_packet_handler;

    std::mutex m_poll_mutex;
    RS::PollManager m_polls;
};

#endif // __CHAT_SERVER_HPP__
