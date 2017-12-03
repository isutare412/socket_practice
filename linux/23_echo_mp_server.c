#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define LISTEN_QUEUE_SIZE 10
#define BUF_SIZE 30

void handler_sigchld(int sig);
void handle_error(const char* msg);
void serve_client(int socket);

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // set signal handler info
    struct sigaction sa;
    sa.sa_handler = handler_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    // set signal handler for SIGCHLD
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        handle_error("sigaction()");
    }

    // make socket for listen
    int serv_sock;
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        handle_error("socket()");
    }

    // set socket address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // set socket option that reuses time-waiting port
    int addr_reuse = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (int*)&addr_reuse, sizeof(addr_reuse)) == -1) {
        handle_error("setsockopt()");
    }

    // bind address to socket
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("bind()");
    }

    // now listen to clients
    if (listen(serv_sock, LISTEN_QUEUE_SIZE) == -1) {
        handle_error("listen()");
    }

    while (1) {
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_sz = sizeof(clnt_addr);

        // accept client
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        if (clnt_sock == -1) {
            perror("accept()");
            fprintf(stderr, "failed accept(); ip(%s), port(%d)\n",
                    inet_ntoa(clnt_addr.sin_addr),
                    ntohs(clnt_addr.sin_port));
            continue;
        }
        printf("client connected; ip(%s), port(%d)\n",
                inet_ntoa(clnt_addr.sin_addr),
                ntohs(clnt_addr.sin_port));

        // create child process to service client
        pid_t pid = fork();
        if (pid == -1) {
            close(clnt_sock);
            continue;
        }

        if (pid == 0) { // child process
            close(serv_sock);

            // service for client
            serve_client(clnt_sock);

            // close connection from client
            close(clnt_sock);
            printf("client disconnected; ip(%s), port(%d)\n",
                    inet_ntoa(clnt_addr.sin_addr),
                    ntohs(clnt_addr.sin_port));
            return 0;
        }

        // parent process
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}

void handler_sigchld(int sig)
{
    if (sig != SIGCHLD) {
        fputs("unexpected signal; expected(SIGCHLD)", stderr);
        exit(1);
    }

    int exit_stat;
    pid_t pid = waitpid(-1, &exit_stat, WNOHANG);
    if (!WIFEXITED(exit_stat)) {
        fputs("Child exited abnormally!!", stderr);
        exit(1);
    }
    
    int exitno = WEXITSTATUS(exit_stat);
    printf("Child exited; pid(%d), exit(%d)\n", pid, exitno);
}

void handle_error(const char* msg)
{
    perror(msg);
    exit(1);
}

void serve_client(int socket)
{
    // read and echo client
    int str_len;
    char buf[BUF_SIZE];
    while ((str_len = read(socket, buf, BUF_SIZE)) != 0) {
        write(socket, buf, str_len);
    }

}
