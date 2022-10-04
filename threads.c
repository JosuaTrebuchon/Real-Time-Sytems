// clang -lpthread main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

long g_count = 0;

// Note the argument and return types: void*
void *fn(void *args)
{
    long l_count = 0;
    for (int i = 0; i < 1000000; i++)
    {
        g_count++;
        l_count++;
    }
    printf("global: %lu\n", g_count);
    printf("local:  %lu\n", l_count);

    return NULL;
}
int main()
{
    pthread_t threads[2];

    pthread_t threadHandle;
    pthread_create(&threads[0], NULL, fn, NULL);
    pthread_create(&threads[1], NULL, fn, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
}
