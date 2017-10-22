#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

void handle_error(const char* msg);

int main()
{
    int write_fd;
    int read_fd;
    char wr_content[256];
    char rd_content[256];

    printf("enter file content: ");
    fgets(wr_content, sizeof(wr_content), stdin);

    // WRITE
    write_fd = open("03_out.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (write_fd < 0)
        handle_error("open()");
    printf("write file descriptor: %d\n", write_fd);

    if (write(write_fd, wr_content, strlen(wr_content)) < 0)
        handle_error("write()");

    // READ
    read_fd = open("03_out.txt", O_RDONLY);
    if (read_fd < 0)
        handle_error("open()");
    printf("read file descriptor: %d\n", read_fd);

    if (read(read_fd, (void*)rd_content, sizeof(rd_content)) < 0)
        handle_error("read()");
    printf("read file: %s", rd_content);

    // CLOSE
    if (close(write_fd) < 0)
        handle_error("close()");

    if (close(read_fd) < 0)
        handle_error("close()");
    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}
