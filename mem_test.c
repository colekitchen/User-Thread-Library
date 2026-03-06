#include <stdio.h>
#include <stdlib.h>
#include "mythreads.h"

void *t1(void *arg)
{
    threadYield();

    printf("Yielded\n");

    threadYield();

    threadExit(NULL);
}

int main()
{
    int id1;
    int p1 = 16;
    void *result1;

    threadInit();

    for (int i = 0; i < 100; i++) {
        id1 = threadCreate(t1, &p1);
        printf("created thread %d\n", id1);

        threadJoin(id1, &result1);
        printf("joined thread %d\n", id1);
    }
}