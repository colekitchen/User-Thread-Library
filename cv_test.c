#include <stdio.h>
#include <stdlib.h>
#include "mythreads.h"

#define N 5

mutexlock_t *lock;
condvar_t *cv;

void *consumer(void *arg)
{
    int id = *((int*)arg);

    threadLock(lock);

    printf("consumer %d waiting\n", id);

    threadWait(lock, cv);

    printf("consumer %d resumed\n", id);

    threadUnlock(lock);

    return NULL;
}

void *producer(void *arg)
{
    for (int i = 0; i < N; i++)
    {
        threadLock(lock);

        printf("producer signaling\n");

        threadSignal(lock, cv);

        threadUnlock(lock);

        threadYield(); // let consumer run
    }

    return NULL;
}

int main()
{
    threadInit();

    lock = lockCreate();
    cv = condvarCreate();

    int ids[N];

    for (int i = 0; i < N; i++)
    {
        ids[i] = i;
        threadCreate(consumer, &ids[i]);
    }

    threadCreate(producer, NULL);

    // allow threads to run
    for (int i = 0; i < 100; i++)
        threadYield();

    printf("test finished\n");

    return 0;
}