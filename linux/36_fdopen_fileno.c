#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    int fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open()");
        return EXIT_FAILURE;
    }

    printf("First file descriptor: %d \n", fd);

    FILE* fp = fdopen(fd, "w");
    if (fp == NULL) {
        perror("fdopen()");
        return EXIT_FAILURE;
    }

    fputs("TCP/IP SOCKET PROGRAMMING\n", fp);
    printf("Second file descriptor: %d \n", fileno(fp));

    fclose(fp);

    return EXIT_SUCCESS;
}
