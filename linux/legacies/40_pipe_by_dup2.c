#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    // open file
    FILE* fp = fopen("pipe_out.txt", "w");
    if (fp == NULL) {
        perror("fopen()");
        return EXIT_FAILURE;
    }

    // pipe stdout to file input
    if (dup2(fileno(fp), STDOUT_FILENO) == -1) {
        perror("dup2()");
        return EXIT_FAILURE;
    }

    execl("/bin/ls", "ls", "-l", (char*)NULL);

    fclose(fp);
    return EXIT_SUCCESS;
}
