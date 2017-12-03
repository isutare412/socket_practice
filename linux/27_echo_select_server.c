#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define LISTEN_QUEUE_SIZE 5
#define CONNECTION_LOG_SIZE 64
#define BUF_SIZE 100

int accept_client(int serv_sock);
int serv_client(int clnt_sock);
void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
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
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (int*)&addr_reuse, sizeof(addr_reuse)) == -1) {
        handle_error("setsockopt()");
    }

    // bind socket
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("bind()");
    }

    // set fd_set for select function
    int fd_max;
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    // listen for client
    if (listen(serv_sock, LISTEN_QUEUE_SIZE) == -1) {
        handle_error("listen()");
    }

    while (1) {
        // fd_set and timeout setting for select
        fd_set copy_reads;
        struct timeval timeout;
        copy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // select file descriptors
        int fd_num;
        fd_num = select(fd_max + 1, &copy_reads, 0, 0, &timeout);
        if (fd_num == -1) {
            handle_error("select()");
        }
        if (fd_num == 0) { // timeout
            continue;
        }

        for (int i = 0; i < fd_max + 1; ++i) {
            if (FD_ISSET(i, &copy_reads)) {
                if (i == serv_sock) { // connection request from client
                    int clnt_sock = accept_client(serv_sock);
                    FD_SET(clnt_sock, &reads);
                    fd_max = fd_max < clnt_sock ? clnt_sock : fd_max;
                }
                else { // echo service for client
                    int sock = serv_client(i);
                    if (sock != i) {
                        continue;
                    }

                    FD_CLR(i, &reads);
                    close(i);
                    printf("client disconnected...\n");
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}

int accept_client(int serv_sock)
{
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_sz;
    clnt_addr_sz = sizeof(clnt_addr);

    int clnt_sock;
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
    if (clnt_sock == -1) {
        handle_error("accept()");
    }

    char log_buf[CONNECTION_LOG_SIZE];
    const char* addr = inet_ntoa(clnt_addr.sin_addr);
    snprintf(log_buf, CONNECTION_LOG_SIZE, "client connected: %s:%d", addr, ntohs(clnt_addr.sin_port));

    printf("%s\n", log_buf);

    return clnt_sock;
}

int serv_client(int clnt_sock)
{
    char buf[BUF_SIZE];

    int str_len = read(clnt_sock, buf, BUF_SIZE - 1);
    if (str_len == -1) {
        handle_error("read()");
    }
    else if (str_len == 0) {
        return clnt_sock;
    }

    write(clnt_sock, buf, str_len);
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
