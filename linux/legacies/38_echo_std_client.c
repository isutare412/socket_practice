#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // make socket
    int sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        handle_error("socket()");
    }

    // set socket address 
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("connect()");
    }

    // log connection
    printf("connected to server; ip(%s), port(%d)\n",
            inet_ntoa(serv_addr.sin_addr),
            ntohs(serv_addr.sin_port));

    // get FILE structure pointer from file descriptor
    FILE* readfp = fdopen(sock, "r");
    FILE* writefp = fdopen(sock, "w");

    char message[BUFSIZ];
    while (1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUFSIZ, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }

        fputs(message, writefp);
        fflush(writefp);
        fgets(message, BUFSIZ, readfp);
        printf("Message from server: %s", message);
    }
    fclose(readfp);
    fclose(writefp);

    return EXIT_SUCCESS;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
