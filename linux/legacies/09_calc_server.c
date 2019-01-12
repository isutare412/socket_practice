#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define CLIENT_INFO_BUF_SIZE 32
#define OP_COUNT_SIZE 4
#define OPEREND_SIZE 4
#define OPEREND_ARR_SIZE 100
#define OPERATOR_SIZE 1

int calculate(char operator, int operends[], int size);
int read_nbytes(int socket, void* dst, int size);
void handle_error(const char* msg);

int main(int argc, char* argv[])
{
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_len;
    int serv_sock, clnt_sock;
    int addr_reuse_opt = 1;
    char clnt_info_buf[CLIENT_INFO_BUF_SIZE];

    int op_count;
    char operator;
    int operends[OPEREND_ARR_SIZE];

    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (int*)&addr_reuse_opt,
            sizeof(addr_reuse_opt)) == -1)
        handle_error("setsockopt()");

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("bind()");

    if (listen(serv_sock, 20) == -1)
        handle_error("listen()");

    while (1) {
        clnt_addr_len = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1)
            handle_error("accept()");

        snprintf(clnt_info_buf, sizeof(clnt_info_buf), "%s:%d",
                inet_ntoa(clnt_addr.sin_addr), clnt_addr.sin_port);
        printf("client connected: %s\n", clnt_info_buf);

        if (read_nbytes(clnt_sock, &op_count, OP_COUNT_SIZE) == -1)
            handle_error("read_nbytes(); op_count");

        if (read_nbytes(clnt_sock, operends, (op_count * OPEREND_SIZE)) == -1)
            handle_error("read_nbytes(); operends");

        if (read_nbytes(clnt_sock, &operator, OPERATOR_SIZE) == -1)
            handle_error("read_nbytes(); operator");

        int result = calculate(operator, operends, op_count);
        if (write(clnt_sock, &result, sizeof(result)) == -1)
            handle_error("write()");

        close(clnt_sock);
    }

    close(serv_sock);
    return 0;
}

int calculate(char operator, int operends[], int size)
{
    int result;

    switch (operator) {
        case '+':
            result = 0;
            for (int i = 0; i < size; ++i) {
                result += operends[i];
            }
            break;
        case '*':
            result = 1;
            for (int i = 0; i < size; ++i) {
                result *= operends[i];
            }
            break;
    }
    return result;
}

int read_nbytes(int socket, void* dst, int size)
{
    char* dest  = (char*)dst;
    int recv_pos, recv_size;

    memset(dst, 0, size);

    recv_pos = 0;
    while (recv_pos < size) {
        recv_size = read(socket, dest + recv_pos, size - recv_pos);
        if (recv_size == -1)
            return -1;
        recv_pos += recv_size;
    }
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
