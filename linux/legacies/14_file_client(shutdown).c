#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define RECV_BUF_SIZE 256

void handle_error(const char* msg);

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    // open TCP socket
    int sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        handle_error("TCP socket creation error");
    }

    // set server address to connect
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("connect error");
    }

    // open file to write data received from server
    FILE* fp;
    fp = fopen("14_file_received.txt", "w");

    // write received data to file until eof
    char recv_buf[RECV_BUF_SIZE];
    int read_cnt;
    do {
        read_cnt = read(sock, recv_buf, RECV_BUF_SIZE);
        if (read_cnt == -1) {
            handle_error("read error");
        }
        fwrite(recv_buf, sizeof(char), read_cnt, fp);
    } while (read_cnt != 0);
    puts("Received file data completely");

    // send response to the server
    static const char response[] = "Thank you!";
    if (write(sock, response, strlen(response)) == -1) {
        handle_error("write error");
    }

    fclose(fp);
    close(sock);
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
