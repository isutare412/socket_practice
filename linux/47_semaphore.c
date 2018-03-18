#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void* read(void*);
void* accumulate(void*);

static sem_t sem_in;
static sem_t sem_out;

static int num;

int main()
{
    sem_init(&sem_in, 0, 1);
    sem_init(&sem_out, 0, 0);

    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, read, NULL);
    pthread_create(&tid2, NULL, accumulate, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    sem_destroy(&sem_in);
    sem_destroy(&sem_out);

    return EXIT_SUCCESS;
}

void* read(void* arg)
{
    for (int i = 0; i < 5; ++i) {
        fputs("Input num: ", stdout);

        sem_wait(&sem_in);
        scanf("%d", &num);
        sem_post(&sem_out);
    }
    return NULL;
}

void* accumulate(void* arg)
{
    int sum = 0;
    for (int i = 0; i < 5; ++i) {
        sem_wait(&sem_out);
        sum += num;
        sem_post(&sem_in);
    }

    printf("Result: %d\n", sum);
    return NULL;
}
