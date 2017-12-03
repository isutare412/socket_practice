#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 32

int main(int argc, const char* arv[])
{
    int fds[2];
    pipe(fds);

    pid_t pid = fork();
    if (pid == -1) {
        perror("pid()");
        exit(1);
    }

    if (pid == 0) { // child process
        const char content[] = "Hi! I'm content for pipe";
        write(fds[1], content, sizeof(content));
        return 0;
    }

    char buf[BUF_SIZE];
    read(fds[0], buf, BUF_SIZE);
    puts(buf);
    return 0;
}
