#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 3

void handle_error(const char* msg);

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source file> <destination file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // get source file descriptor
    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) {
        handle_error("open(argv[1])");
    }

    // get destination file descriptor
    int fd2 = open(argv[2], O_WRONLY |
                            O_CREAT |
                            O_TRUNC, 0644);
    if (fd2 == -1) {
        handle_error("open(argv[2])");
    }

    // copy from fd1 to fd2
    char buf[BUF_SIZE];
    int len;
    while((len = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, len);
    }

    // close file descriptors
    close(fd1);
    close(fd2);

    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
