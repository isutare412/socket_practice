#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "chat_server.hpp"

static constexpr int LISTEN_QUEUE_SIZE = 30;

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ChatServer chat_server;
    if (!chat_server.initialize(atoi(argv[1]), LISTEN_QUEUE_SIZE))
    {
        fprintf(stderr, "failed to initailize chat_server\n");
        exit(EXIT_FAILURE);
    }

    chat_server.run();

    return EXIT_SUCCESS;
}