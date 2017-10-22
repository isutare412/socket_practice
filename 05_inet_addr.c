#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    const char* addr1 = "1.2.3.4";
    const char* addr2 = "127.0.0.1";
    char addr3[16];
    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0, sizeof(sock_addr));

    // inet_addr
    uint32_t conv_addr = inet_addr(addr1);
    printf("%-9s -> %x\n", addr1, conv_addr);

    // inet_aton
    inet_aton(addr2, &sock_addr.sin_addr);
    printf("%-9s -> %x\n", addr2, sock_addr.sin_addr.s_addr);

    // inet_ntoa
    strlcpy(addr3, inet_ntoa(sock_addr.sin_addr), sizeof(addr3));
    printf("%-9x -> %s\n", sock_addr.sin_addr.s_addr, addr3);
    return 0;
}
