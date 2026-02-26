#include <ucontext.h>
#include <stddef.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include "mythreads.h"

struct Thread {
    ucontext_t thread;
    void *stack;
    int status;
};

ucontext_t main_context;
void **returnValues;
struct Thread *threads;
int current_thread, recent_thread, numOfThreads;

void threadStubFunction(thFuncPtr funcPtr, void *argPtr) {
    threads[current_thread].status = 0;
    void *returnValue = funcPtr(argPtr);
    if (numOfThreads != 0) {
        returnValues = realloc(returnValues, (numOfThreads+1) * sizeof(void *));
        returnValues[numOfThreads] = NULL;
    }
    returnValues[current_thread] = returnValue;

    swapcontext(&threads[current_thread].thread, &main_context);
}

void threadInit(void) {
    threads = malloc(1 * sizeof(struct Thread));
    returnValues = calloc(1, sizeof(void *));
    numOfThreads = 0;
    recent_thread = 0;
    current_thread = 0;
    return;
}

int threadCreate(thFuncPtr funcPtr, void *argPtr) {
    if (numOfThreads != 0) {
        threads = realloc(threads, (numOfThreads+1) * sizeof(struct Thread));
    }
    if (getcontext(&threads[numOfThreads].thread) == -1) {
        perror("Failed to get context");
    }

    threads[numOfThreads].stack = malloc(STACK_SIZE);
    threads[numOfThreads].thread.uc_stack.ss_sp = threads[numOfThreads].stack;
	threads[numOfThreads].thread.uc_stack.ss_size = STACK_SIZE;

    makecontext(&threads[numOfThreads].thread, (void (*)(void))threadStubFunction, 2, funcPtr, argPtr);

    current_thread = numOfThreads;

    swapcontext(&main_context, &threads[numOfThreads].thread);

    threads[current_thread].status = 1;

    return numOfThreads++;
}
void threadYield(void) {
    recent_thread = current_thread;

    for (int i = 0; i < numOfThreads+1; i++) {
        if (threads[i].status == 0 && i != recent_thread) {
            current_thread = i;
            break;
        }
    }
    swapcontext(&threads[recent_thread].thread, &threads[current_thread].thread);
    return;
}
void threadJoin(int thread_id, void **result) {
    return;
}

//exits the current thread -- closing the main thread, will terminate the program
void threadExit(void *result) {
    return;
}