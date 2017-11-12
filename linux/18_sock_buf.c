#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    int sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);

    // get default socket send/receive buffer size
    int snd_buf, rcv_buf;
    socklen_t len;
    len = sizeof(snd_buf);
    if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len) != 0) {
        handle_error("getsockopt()");
    }
    len = sizeof(rcv_buf);
    if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len) != 0) {
        handle_error("getsockopt()");
    }

    printf("Input buffer size: %d\n", rcv_buf);
    printf("Output buffer size: %d\n", snd_buf);
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
