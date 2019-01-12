#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 128

void handle_error(const char* msg);

int main(int argc, const char* argv[])
{
    if (argc != 4) {
        printf("Usage: %s <GroupIP> <PORT> <file name>\n", argv[0]);
        exit(1);
    }

    // make socket
    int send_sock;
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);

    // set socket address
    struct sockaddr_in mul_addr;
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_family = AF_INET;
    mul_addr.sin_addr.s_addr = inet_addr(argv[1]);  // multicast IP
    mul_addr.sin_port = htons(atoi(argv[2]));       // multicast Port

    // set TTL info for socket
    int time_live = TTL;
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));

    // open file to multicast
    FILE* fp;
    if ((fp = fopen(argv[3], "r")) == NULL) {
        handle_error("fopen()");
    }

    // multicast file line by line
    char buf[BUF_SIZE];
    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&mul_addr, sizeof(mul_addr));
        printf("%s", buf);
        sleep(1);
    }

    // close file and socket
    fclose(fp);
    close(send_sock);

    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
