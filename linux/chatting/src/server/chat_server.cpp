#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <thread>
#include <fstream>

#include "common/network/rssocket.hpp"
#include "common/network/tools.hpp"
#include "chat_server.hpp"

ChatServer::ChatServer() noexcept
    : m_server_socket(0),
    m_threads(std::thread::hardware_concurrency())
{
    memset(&m_server_addr, 0, sizeof(m_server_addr));

    uint32_t conSize = std::thread::hardware_concurrency();
    printf("ChatServer allocated %u threads\n", conSize);
}

ChatServer::~ChatServer()
{
    m_socket_manager.for_each_socket(
        [](int socket)
        {
            close(socket);
        }
    );

    m_socket_manager.clear();
    close(m_server_socket);
}

bool
ChatServer::initialize(
    in_port_t port,
    uint32_t listen_queue_size
) noexcept
{
    int server_socket = RS::socket();
    m_server_socket = server_socket;
    printf("socket created; socket_fd(%d)\n", server_socket);

    sockaddr_in sock_addr = RS::make_sockaddr(
        htonl(INADDR_ANY), htons(port)
    );
    m_server_addr = sock_addr;

    RS::setsockopt(server_socket, RS::SocketOption::REUSEADDR);
    RS::bind(server_socket, sock_addr);
    printf("bind address to socket; socket_fd(%d) address(%s)\n",
        server_socket, RS::sockaddr_to_string(sock_addr).c_str());

    RS::listen(server_socket, listen_queue_size);
    printf("start to listen; socket_fd(%d) queue_size(%d)\n",
        server_socket, listen_queue_size);

    return true;
}

void
ChatServer::run() noexcept
{
    while (true)
    {
        sockaddr_in clnt_addr;
        int clnt_sock = RS::accept(m_server_socket, clnt_addr);
        printf("client connected; address(%s)\n",
            RS::sockaddr_to_string(clnt_addr).c_str());

        m_socket_manager.register_socket(clnt_sock, clnt_addr);
        m_threads.enqueue(
            [this, clnt_sock]
            {
                send_file(clnt_sock, "test.txt");
            }
        );
    }
}

void
ChatServer::close_client(
    int socket
) noexcept
{
    if (!m_socket_manager.is_socket_registered(socket))
    {
        return;
    }

    sockaddr_in address = m_socket_manager.get_sockaddr(socket);

    close(socket);
    m_socket_manager.unregister_socket(socket);

    printf("client closed; socket(%d) address(%s)\n",
        socket, RS::sockaddr_to_string(address).c_str());
}

void
ChatServer::serv_client(
    int sock_fd
) noexcept
{
    // TODO
}

void
ChatServer::send_file(
    int sock_fd,
    const char* filename
) noexcept
{
    std::ifstream ifs(filename);
    if (ifs.fail())
    {
        printf("file does not exists; file(%s)\n", filename);
        return;
    }

    std::string filebody(
        (std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>()
    );
    ifs.close();

    sockaddr_in address = m_socket_manager.get_sockaddr(sock_fd);

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
