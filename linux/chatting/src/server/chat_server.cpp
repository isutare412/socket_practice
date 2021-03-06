#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <thread>
#include <fstream>

#include "common/network/socket.hpp"
#include "common/network/tools.hpp"
#include "chat_server.hpp"

ChatServer::ChatServer() noexcept
    : m_server_socket(-1),
    m_threads(std::thread::hardware_concurrency())
{
    memset(&m_server_addr, 0, sizeof(m_server_addr));

    uint32_t conSize = std::thread::hardware_concurrency();
    printf("ChatServer allocated %u threads\n", conSize);
}

ChatServer::~ChatServer()
{
    m_session_manager.clear();

    {
        std::lock_guard<std::mutex> lock(m_poll_mutex);
        m_polls.clear();
    }
}

bool ChatServer::initialize(in_port_t port, uint32_t listen_queue_size) noexcept
{
    int server_socket = RS::socket();
    printf("socket created; socket_fd(%d)\n", server_socket);

    sockaddr_in sock_addr = RS::make_sockaddr(
        htonl(INADDR_ANY), htons(port)
    );

    RS::setsockopt(server_socket, RS::SocketOption::REUSEADDR);
    RS::bind(server_socket, sock_addr);
    printf("bind address to socket; socket_fd(%d) address(%s)\n",
        server_socket, RS::sockaddr_to_string(sock_addr).c_str());

    RS::listen(server_socket, listen_queue_size);
    printf("start to listen; socket_fd(%d) queue_size(%d)\n",
        server_socket, listen_queue_size);

    m_server_socket = server_socket;
    m_server_addr = sock_addr;
    register_socket(server_socket, sock_addr);

    return true;
}

bool ChatServer::run() noexcept
{
    if (!m_threads.run(true))
    {
        return false;
    }

    while (true)
    {
        std::vector<pollfd> poll_result;
        {
            std::lock_guard<std::mutex> lock(m_poll_mutex);

            // wait until at least one socket is polled
            const std::vector<pollfd>* res = m_polls.poll(-1);
            if (res == nullptr)
            {
                continue;
            }

            // return copy of poll_results to protect m_poll_manager by m_mutex
            poll_result = *res;
        }

        for (const pollfd& status : poll_result)
        {
            const int socket = status.fd;
            const short revents = status.revents;

            if (revents == 0)
            {
                continue;
            }
            // the socket has been disconnected 
            else if (revents & POLLHUP)
            {
                close_client(socket);
            }
            else if (revents & POLLIN)
            {
                // accept client from server socket
                if (socket == m_server_socket)
                {
                    accept_client(socket);
                }
                // handle client socket
                else
                {
                    // m_threads.enqueue([this, socket]
                    // {
                    //     echo_client(socket);
                    // });

                    //echo_client(socket);

                    handle_client(socket);
                }
            }
        }
    }

    return true;
}

bool ChatServer::handle_client(int socket) noexcept
{
    int32_t type, size;
    RS::ErrorType error = RS::read_nbytes(socket, &type, sizeof(type));
    if (error != RS::ErrorType::NO_ERROR)
    {
        printf("failed to handle client; type failed\n");
        return false;
    }

    error = RS::read_nbytes(socket, &size, sizeof(size));
    if (error != RS::ErrorType::NO_ERROR)
    {
        printf("failed to handle client; size failed\n");
        return false;
    }

    type = ntohl(type);
    size = ntohl(size);

    char packet_buf[SERIALIZE_BUF_SIZE];
    error = RS::read_nbytes(socket, &packet_buf, size);
    if (error != RS::ErrorType::NO_ERROR)
    {
        printf("failed to handle client; buffer failed\n");
        return false;
    }

    bool result = false;
    m_session_manager.for_session(
        socket,
        [this, type, packet_buf, size, &result](ClientSession* session)
        {
            result = m_client_packet_handler.handle(type, packet_buf, size, session);
        }
    );

    return result;
}

void ChatServer::accept_client(int socket) noexcept
{
    sockaddr_in clnt_addr;
    int clnt_sock = RS::accept(socket, clnt_addr);
    printf("client connected; address(%s)\n",
        RS::sockaddr_to_string(clnt_addr).c_str());

    register_socket(clnt_sock, clnt_addr);
}

void ChatServer::close_client(int socket) noexcept
{
    if (!m_session_manager.is_socket_registered(socket))
    {
        return;
    }

    sockaddr_in address = m_session_manager.get_sockaddr(socket);
    m_session_manager.unregister_session(socket);

    {
        std::lock_guard<std::mutex> lock(m_poll_mutex);
        m_polls.unregister_socket(socket);
    }

    printf("client closed; socket(%d) address(%s)\n",
        socket, RS::sockaddr_to_string(address).c_str());
}

void ChatServer::register_socket(int socket, const sockaddr_in& addr) noexcept
{
    m_session_manager.register_session(socket, addr);

    {
        std::lock_guard<std::mutex> lock(m_poll_mutex);

        if (!m_polls.register_socket(socket, POLLIN | POLLPRI))
        {
            fprintf(stderr,
                "cannot register socket to poll manager; already exists; socket(%d)\n", socket);
        };
    }
}

void ChatServer::echo_client(int sock_fd) noexcept
{
    char buf[256];
    int str_len = read(sock_fd, buf, sizeof(buf) - 1);
    buf[str_len] = '\0';

    write(sock_fd, buf, str_len);

    buf[strcspn(buf, "\r\n")] = '\0';
    printf("relayed message; client(%d) message(%s)\n", sock_fd, buf);
}

void ChatServer::send_file(int sock_fd, const char* filename) noexcept
{
    std::ifstream ifs(filename);
    if (ifs.fail())
    {
        fprintf(stderr, "file does not exists; file(%s)\n", filename);
        return;
    }

    std::string filebody(
        (std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>()
    );
    ifs.close();

    sockaddr_in address = m_session_manager.get_sockaddr(sock_fd);

    if (write(sock_fd, filebody.c_str(), filebody.size()) == -1)
    {
        RS::handle_perror("failed to send file; file(%s) address(%s)",
            filename, RS::sockaddr_to_string(address).c_str());
    }

    printf("sent file to client; file(%s) address(%s)\n",
        filename, RS::sockaddr_to_string(address).c_str()
    );

    close_client(sock_fd);
}
