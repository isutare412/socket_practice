#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "rssocklib.h"

#define MALLOC_SIZE 50

void* thread_main(void* arg);

int main()
{
    pthread_t tid;
    int thread_param = 5;
    if (pthread_create(&tid, NULL, thread_main, (void*)&thread_param) != 0) {
        handle_error("pthread_create()");
    }

    void* thr_ret;
    if (pthread_join(tid, &thr_ret) != 0) {
        handle_error("pthread_join()");
    }

    printf("thread return message: %s \n", (char*)thr_ret);
    free(thr_ret);

    return EXIT_SUCCESS;
}

void* thread_main(void* arg)
{
    int cnt = *((int*)arg);

    char* msg = (char*)malloc(sizeof(char) * MALLOC_SIZE);
    strcpy(msg, "Hello, I'm thread~");

    for (int i = 0; i < cnt; ++i) {
        sleep(1);
        puts("thread running");
    }

    return (void*)msg;
}
