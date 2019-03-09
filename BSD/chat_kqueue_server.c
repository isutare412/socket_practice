#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/event.h>

#include "rssocklib.h"

#define MAX_CLNT 256
#define MSG_BUF_SIZE 4096
#define KQUEUE_SIZE MAX_CLNT

void fd_set_nonblocking(int fd);
void accept_clnt(int sock);
void* handle_clnt(void*);
void send_msg(const char* msg, int len);

static pthread_mutex_t g_mutex;

static uint8_t kqueue_cnt = 0;
static struct kevent event_list[KQUEUE_SIZE];
static int kernel_queue;

static int clnt_cnt = 0;
static int clnt_socks[MAX_CLNT];

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // initialize mutex
    pthread_mutex_init(&g_mutex, NULL);

    // make kernel queue
    kernel_queue = kqueue();
    if (kernel_queue == -1) {
        handle_error("kqueue()");
    }

    // make socket and bind to the port
    int serv_sock = rsserv_sockbind(atoi(argv[1]));

    // set serv_sock as non-blocking file descriptor
    fd_set_nonblocking(serv_sock);

    // set kevent to register
    struct kevent change_event;
    EV_SET(&change_event,
            serv_sock,
            EVFILT_READ,
            EV_ADD | EV_ENABLE | EV_ONESHOT,
            0, 0, 0);
    pthread_mutex_lock(&g_mutex);
    ++kqueue_cnt;
    pthread_mutex_unlock(&g_mutex);

    // register server socket event to kqueue
    int result = kevent(kernel_queue, &change_event, 1, NULL, 0, NULL);
    if (result != -1) {
        handle_error("kevent()");
    }

    // listen to the socket to accept clients
    if (listen(serv_sock, LISTEN_QUEUE_SIZE) == -1) {
        handle_error("listen()");
    }

    for (;;) {
        // get kernel event
        pthread_mutex_lock(&g_mutex);
        int cnt = kevent(kernel_queue,
                NULL, 0,
                event_list, kqueue_cnt,
                NULL);
        pthread_mutex_unlock(&g_mutex);

        if (cnt == -1) {
            handle_error("kevent()");
        }

        for (int i = 0; i < cnt; ++i) {
            int event_fd = event_list[i].ident;
            if (event_fd == serv_sock) {
                accept_clnt(event_fd);
            }
            else {
                pthread_t tid;
                pthread_create(&tid, NULL, handle_clnt, (void*)&event_fd);
                pthread_detach(tid);
            }
        }

        // accept client
        int clnt_sock = rsserv_acceptlog(serv_sock);

        // save accepted client's socket file descriptor for broadcasting
        pthread_mutex_lock(&g_mutex);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&g_mutex);

        // service the client
        pthread_t tid;
        pthread_create(&tid, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(tid);
    }

    close(serv_sock);
    return EXIT_SUCCESS;
}

void fd_set_nonblocking(int fd)
{
    // set fd to non-blocking state
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

void accept_clnt(int sock)
{
    // accept client from server socket and leave connection log
    int clnt_sock = rsserv_acceptlog(sock);

    // set clnt_sock as non-blocking file descriptor
    fd_set_nonblocking(clnt_sock);

    // set kevent to register
    struct kevent change_event;
    EV_SET(&change_event,
            clnt_sock,
            EVFILT_READ,
            EV_ADD | EV_ENABLE | EV_ONESHOT,
            0, 0, 0);
    pthread_mutex_lock(&g_mutex);
    ++kqueue_cnt;
    pthread_mutex_unlock(&g_mutex);

    // register client socket to kqueue
    int result = kevent(kernel_queue, &change_event, 1, NULL, 0, NULL);
    if (result != -1) {
        handle_error("kevent()");
    }
}

void* handle_clnt(void* arg)
{
    const int clnt_sock = *((int*)arg);

    int str_len;
    char msg[MSG_BUF_SIZE];
    str_len = read(clnt_sock, msg, sizeof(msg));

    // broadcast message to other clients
    if (str_len > 0) {
        send_msg(msg, str_len);
        return NULL;
    }

    // socket closed
    if (str_len == 0) {
        // remove disconnected client from clnt_socks
        pthread_mutex_lock(&g_mutex);
        for (int i = 0; i < clnt_cnt; ++i) {
            if (clnt_socks[i] == clnt_sock) {
                while (i < clnt_cnt - 1) {
                    clnt_socks[i] = clnt_socks[i + 1];
                    ++i;
                }
                break;
            }
        }
        --clnt_cnt;
        --kqueue_cnt;
        pthread_mutex_unlock(&g_mutex);
    }

    // close socket of disconnected client
    close(clnt_sock);

    return NULL;
}

void send_msg(const char* msg, int len)
{
    pthread_mutex_lock(&g_mutex);
    for (int i = 0; i < clnt_cnt; ++i) {
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&g_mutex);
}
