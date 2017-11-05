#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ERROR_BUF_SIZE 64
#define LISTEN_QUEUE_SIZE 20
#define CONNECTION_LOG_SIZE 64

#define FILE_BUF_SIZE 16
#define CLIENT_RESPONSE_SIZE 64


static const char FILE_CONTENT[] =
"This is content for file\n\
Just filling a file with useless sentences...\n\
2017/10/29 18:43\n\
by Redshore\n";

void handle_error(const char* msg);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    // open file
    FILE* fp;
    fp = fopen("13_file_to_send.txt", "w+");

    // write contents to the file
    fwrite(FILE_CONTENT, sizeof(char), strlen(FILE_CONTENT), fp);
    fseek(fp, 0L, SEEK_SET);

    // open TCP socket
    int serv_sock;
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        handle_error("TCP socket creation error");
    }

    // set socket address to bind
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // set socket option
    int addr_reuse = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (int*)&addr_reuse,
            sizeof(addr_reuse)) == -1) {
        handle_error("set socket option error");
    }

    // bind address to the socket
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        char err[ERROR_BUF_SIZE];
        snprintf(err, ERROR_BUF_SIZE, "bind error [%s:%u]",
                inet_ntoa(serv_addr.sin_addr), serv_addr.sin_port);
        handle_error(err);
    }

    // listen socket
    if (listen(serv_sock, LISTEN_QUEUE_SIZE) == -1) {
        handle_error("listen error");
    }

    // accept client
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_sz = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
    if (clnt_sock == -1) {
        handle_error("accept error");
    }

    // leave client connection log
    char clnt_log[CONNECTION_LOG_SIZE];
    snprintf(clnt_log, CONNECTION_LOG_SIZE, "client connected: %s:%d",
            inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);
    printf("%s\n", clnt_log);

    // write to client socket repeatedly
    while (!feof(fp)) {
        char buf[FILE_BUF_SIZE];
        int read_cnt;

        read_cnt = fread(buf, sizeof(char), FILE_BUF_SIZE, fp);
        if (ferror(fp)) {
            handle_error("fread error");
        }
        if (write(clnt_sock, buf, read_cnt) == -1) {
            handle_error("write error");
        }
    }

    // inform client that the file sending is complete
    if (shutdown(clnt_sock, SHUT_WR) == -1) {
        handle_error("shutdown failed");
    }

    // read response from client
    char response[CLIENT_RESPONSE_SIZE];
    if (read(clnt_sock, response, CLIENT_RESPONSE_SIZE) == -1) {
        handle_error("read error");
    }
    printf("Message from client: %s\n", response);

    fclose(fp);
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
