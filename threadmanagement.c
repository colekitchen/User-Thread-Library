#include <ucontext.h>
#include <stddef.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include "mythreads.h"

ucontext_t *threads;
char *stacks;
void **returnValues;
int current_thread;

void threadStubFunction(thFuncPtr funcPtr, void *argPtr) {
    void *returnValue = funcPtr(argPtr);
    returnValues = realloc(returnValues, current_thread * sizeof(void *));
    returnValues[current_thread] = returnValue;
    
}

void threadInit(void) {
    threads = malloc(1 * sizeof(ucontext_t));
    stacks = malloc(1 * STACK_SIZE);
    returnValues = malloc(1 * sizeof(void *));
    current_thread = 0;
    return;
}
int threadCreate(thFuncPtr funcPtr, void *argPtr) {
    if (current_thread != 0) {
        threads = realloc(threads, current_thread * sizeof(ucontext_t));
        stacks = (char *)realloc(stacks, current_thread * STACK_SIZE);
    }
    if (getcontext(&threads[current_thread]) == -1) {
        perror("Failed to get context");
    }

    threads[current_thread].uc_stack.ss_sp = stacks[current_thread];
	threads[current_thread].uc_stack.ss_size = STACK_SIZE;

    makecontext(&threads[current_thread], (void (*)())threadStubFunction, 2, funcPtr, argPtr);

    current_thread++;

    return current_thread;
}
void threadYield(void) {
    return;
}
void threadJoin(int thread_id, void **result) {
    return;
}

//exits the current thread -- closing the main thread, will terminate the program
void threadExit(void *result) {
    return;
}