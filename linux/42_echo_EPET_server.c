#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define LISTEN_QUEUE_SIZE 10
#define EPOLL_SIZE 50

int fd_set_nonblocking(int fd);
int accept_routine(int epoll_fd, int sock);
int serve_routine(int epoll_fd, int sock);
void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <port> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // make socket
    int serv_sock;
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        handle_error("socket()");
    }

    // set socket address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // set socket option that reuses time-waiting port
    int addr_reuse = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR,
                &addr_reuse, sizeof(addr_reuse)) == -1) {
        handle_error("setsockopt()");
    }

    // bind address to socket
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("bind()");
    }

    // now listen to socket
    if (listen(serv_sock, LISTEN_QUEUE_SIZE) == -1) {
        handle_error("listen()");
    }

    // create epoll instance
    int epfd;
    epfd = epoll_create(EPOLL_SIZE);
    if (epfd == -1) {
        handle_error("epoll_create()");
    }

    // set serv_sock as non-blocking file descriptor
    fd_set_nonblocking(serv_sock);

    // set epoll event
    struct epoll_event event;
    event.events = EPOLLIN; // level triggered by default
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    // malloc epoll_event array for epoll_wait
    struct epoll_event* ep_events;
    ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    while (1) {
        // wait for event from epfd(epfd pointing serv_sock)
        int event_cnt;
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1) {
            handle_error("epoll_wait()");
        }

        // accept/serve clients
        for (int i = 0; i < event_cnt; ++i) {
            int event_fd = ep_events[i].data.fd;
            if (event_fd == serv_sock) {
                accept_routine(epfd, event_fd); // accept
            }
            else {
                serve_routine(epfd, event_fd);  // echo
            }
        }
    }

    // close file descriptors
    close(serv_sock);
    close(epfd);

    return EXIT_SUCCESS;
}

int fd_set_nonblocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

int accept_routine(int epoll_fd, int sock)
{
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    clnt_addr_size = sizeof(clnt_addr);

    // accept client
    int clnt_sock;
    clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
        perror("accept()");
        return -1;
    }

    // log client connection
    printf("client connected; ip(%s), port(%d)\n",
            inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    // set clnt_sock as non-blocking file descriptor
    fd_set_nonblocking(clnt_sock);

    // register clnt_sock to epoll_event
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = clnt_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clnt_sock, &event) == -1) {
        perror("epoll_ctl(EPOLL_CTL_ADD) from accept_routine()");
        return -1;
    }
    return 0;
}

int serve_routine(int epoll_fd, int sock)
{
    while (1) {
        char buf[BUFSIZ];
        int str_len = read(sock, buf, BUFSIZ);

        // socket closed
        if (str_len == 0) {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock, NULL) == -1) {
                perror("epoll_ctl(EPOLL_CTL_DEL) from serve_routine");
                close(sock);
                return -1;
            }

            close(sock);
            printf("client disconneted; fd(%d) \n", sock);
            return 0;
        }

        // read all buffer
        if (str_len == -1 && errno == EAGAIN) {
            break;
        }

        // echo message
        write(sock, buf, str_len);
    }

    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
