#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define OP_COUNT_SIZE 4
#define OPEREND_SIZE 4
#define OPEREND_ARR_SIZE 100
#define OPERATOR_SIZE 1

int read_nbytes(int socket, void* dst, int size);
void handle_error(const char* msg);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;

    int op_count;
    char operator;
    int operends[OPEREND_ARR_SIZE];
    int result;

    if (argc != 3) {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
        handle_error("socket()");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        handle_error("connect()");

    printf("server connected...\n");

    printf("operend count: ");
    scanf("%d", &op_count);

    for (int i = 0; i < op_count; ++i) {
        printf("operend %d: ", i + 1);
        scanf("%d", operends + i);
    }

    while (1) {
        printf("operator(+/*): ");
        scanf(" %c", &operator);
        if (operator == '+' || operator == '*')
            break;
    }

    if (write(sock, &op_count, OP_COUNT_SIZE) == -1)
        handle_error("write(); op_count");
    if (write(sock, operends, (op_count * OPEREND_SIZE)) == -1)
        handle_error("write(); operends");
    if (write(sock, &operator, OPERATOR_SIZE) == -1)
        handle_error("write(); operator");

    if (read_nbytes(sock, &result, sizeof(result)) == -1)
        handle_error("read_nbytes()");

    printf("result from server: %d\n", result);

    close(sock);
    return 0;
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
