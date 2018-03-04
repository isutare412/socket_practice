#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    int cfd1, cfd2;
    cfd1 = dup(STDOUT_FILENO);
    cfd2 = dup2(cfd1, 7);

    printf("cfd1 = %d\ncfd2 = %d\n", cfd1, cfd2);

    const char str[] = "Hi~\n";
    write(cfd1, str, sizeof(str));
    write(cfd2, str, sizeof(str));
    close(cfd1);
    close(cfd2);

    write(STDOUT_FILENO, str, sizeof(str));
    close(STDOUT_FILENO);
    write(STDOUT_FILENO, str, sizeof(str));

    return EXIT_SUCCESS;
}
