#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 3

void handle_error(const char* msg);

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source file> <destination file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* fp1 = fopen(argv[1], "r");
    if (fp1 == NULL) {
        handle_error("fopen(argv[1])");
    }
    FILE* fp2 = fopen(argv[2], "w");
    if (fp2 == NULL) {
        handle_error("fopen(argv[2])");
    }

    char buf[BUF_SIZE];
    while (fgets(buf, BUF_SIZE, fp1) != NULL) {
        fputs(buf, fp2);
    }

    fclose(fp1);
    fclose(fp2);

    return 0;
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
