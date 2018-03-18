#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "rssocklib.h"

#define MAX_CLNT 256
#define MSG_BUF_SIZE 4096

void* handle_clnt(void*);
void send_msg(const char* msg, int len);

pthread_mutex_t mutex;

static int clnt_cnt = 0;
static int clnt_socks[MAX_CLNT];

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // make socket and bind to the port
    int serv_sock = rsserv_sockbind(atoi(argv[1]));

    // listen to the socket to accept clients
    if (listen(serv_sock, LISTEN_QUEUE_SIZE) == -1) {
        handle_error("listen()");
    }

    while (1) {
        // accept client
        int clnt_sock = rsserv_acceptlog(serv_sock);

        // save accepted client's socket file descriptor for broadcasting
        pthread_mutex_lock(&mutex);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutex);

        // service the client
        pthread_t tid;
        pthread_create(&tid, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(tid);
    }

    close(serv_sock);
    return EXIT_SUCCESS;
}

void* handle_clnt(void* arg)
{
    const int clnt_sock = *((int*)arg);

    // broadcast message to other clients
    int str_len;
    char msg[MSG_BUF_SIZE];
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
        send_msg(msg, str_len); // broadcast
    }

    // remove disconnected client from clnt_socks
    pthread_mutex_lock(&mutex);
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
    pthread_mutex_unlock(&mutex);

    // close socket of disconnected client
    close(clnt_sock);

    return NULL;
}

void send_msg(const char* msg, int len)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clnt_cnt; ++i) {
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutex);
}
