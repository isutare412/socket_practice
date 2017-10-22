#include <stdio.h>
#include <arpa/inet.h>

int main()
{
    unsigned int host_addr = 0x12345678;
    unsigned int net_addr;

    char* byte_ptr;

    byte_ptr = (char*)&host_addr;
    printf("%-9s: 0x", "host_addr");
    for (int i = 0; i < sizeof(host_addr); ++i) {
        printf("%x", *byte_ptr++);
    }
    printf("\n");

    net_addr = htonl(host_addr);
    byte_ptr = (char*)&net_addr;
    printf("%-9s: 0x", "net_addr");
    for (int i = 0; i < sizeof(net_addr); ++i) {
        printf("%x", *byte_ptr++);
    }
    printf("\n");
    return 0;
}
