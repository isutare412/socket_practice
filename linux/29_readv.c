#include <stdio.h>
#include <unistd.h>
#include <sys/uio.h>

#define BUF_SIZE 128

int main()
{
    struct iovec vec[2];
    char buf1[BUF_SIZE] = {0,};
    char buf2[BUF_SIZE] = {0,};

    vec[0].iov_base = buf1;
    vec[0].iov_len = 5;
    vec[1].iov_base = buf2;
    vec[1].iov_len = BUF_SIZE;

    int str_len;
    str_len = readv(STDIN_FILENO, vec, sizeof(vec) / sizeof(vec[0]));
    printf("Read bytes: %d \n", str_len);
    printf("First  buffer: %s \n", vec[0].iov_base);
    printf("Second buffer: %s \n", vec[1].iov_base);
    return 0;
}
