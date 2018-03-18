#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "rssocklib.h"

#define MSG_BUF_SIZE 4096
#define NAME_SIZE 32

void* send_msg(void*);
void* recv_msg(void*);

static char name[NAME_SIZE] = "[default]";
static char msg[MSG_BUF_SIZE];

int main(int argc, const char* argv[])
{
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // set my chat name
    snprintf(name, NAME_SIZE, "[%s]", argv[3]);

    // connect to the server
    int sock = rsclnt_sockconnect(argv[1], atoi(argv[2]));

    puts("enter Q for exit");

    // start chatting
    pthread_t snd_thread, rcv_thread;
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, NULL);
    pthread_join(rcv_thread, NULL);

    // close socket
    close(sock);

    return EXIT_SUCCESS;
}

void* send_msg(void* arg)
{
    const int sock = *((int*)arg);
    char name_msg[NAME_SIZE + MSG_BUF_SIZE];

    while (1) {
        fgets(msg, MSG_BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            exit(EXIT_SUCCESS);
        }

        snprintf(name_msg, sizeof(name_msg), "%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
void* recv_msg(void* arg)
{
    const int sock = *((int*)arg);
    char name_msg[NAME_SIZE + MSG_BUF_SIZE];

    int str_len;
    while (1) {
        str_len = read(sock, name_msg, sizeof(name_msg) - 1);
        if (str_len == -1) {
            handle_error("read()");
        }
        name_msg[str_len] = '\0';
        puts(name_msg);
    }
    return NULL;
}
