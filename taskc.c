// clang -lpthread main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

long g_count = 0;
sem_t mtx;

// Note the argument and return types: void*
void *fn(void *args)
{
    long l_count = 0;
    for (int i = 0; i < 1000000; i++)
    {
        sem_wait(&mtx);
        g_count++;
        sem_post(&mtx);
        l_count++;
    }
    sem_wait(&mtx);
    printf("global: %lu\n", g_count);
    sem_post(&mtx);
    printf("local:  %lu\n", l_count);

    return NULL;
}
int main()
{
    sem_init(&mtx, 0, 1);

    pthread_t threads[2];

    pthread_t threadHandle;
    pthread_create(&threads[0], NULL, fn, NULL);
    pthread_create(&threads[1], NULL, fn, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    sem_destroy(&mtx);
}
