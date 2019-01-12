#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define LISTEN_QUEUE_SIZE 8

void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
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
                (void*)&addr_reuse, sizeof(addr_reuse)) == -1) {
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

    // variables for client information
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len;
    int clnt_sock;
    char message[BUFSIZ];

    while (1) {
        // accept client
        clnt_addr_len = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1) {
            handle_error("accept()");
        }

        // log client information
        printf("client connected; ip(%s), port(%d)\n",
                inet_ntoa(clnt_addr.sin_addr),
                ntohs(clnt_addr.sin_port));

        // get FILE structure pointer from file descriptor
        FILE* readfp = fdopen(clnt_sock, "r");
        FILE* writefp = fdopen(clnt_sock, "w");

        // copy from readfp to writefp
        while (!feof(readfp)) {
            fgets(message, BUFSIZ, readfp);
            fputs(message, writefp);
            fflush(writefp); // flush writefp to send immediately
        }

        fclose(readfp);
        fclose(writefp);
    }

    close(serv_sock);

    return EXIT_SUCCESS;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
