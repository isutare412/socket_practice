#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREAD 100

void* thread_inc(void*);
void* thread_dec(void*);

static pthread_mutex_t mutex;

static int64_t num = 0;

int main()
{
    pthread_t tid[NUM_THREAD];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREAD; ++i) {
        if (i % 2) {
            pthread_create(&tid[i], NULL, thread_inc, NULL);
        }
        else {
            pthread_create(&tid[i], NULL, thread_dec, NULL);
        }
    }

    for (int i = 0; i < NUM_THREAD; ++i) {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("result: %lld\n", num);
    return EXIT_SUCCESS;
}

void* thread_inc(void* arg)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 50000; ++i) {
        num += 1;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* thread_dec(void* arg)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 50000; ++i) {
        num -= 1;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
