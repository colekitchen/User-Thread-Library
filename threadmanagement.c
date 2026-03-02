#include <ucontext.h>
#include <stddef.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include "mythreads.h"

//mutex lock types and functions.
struct mutexlock {
    int lock;
}; //opaque type -- you need to implement this in your source file
typedef struct mutexlock mutexlock_t;

struct Thread {
    ucontext_t thread;
    void *stack;
    int status;
    int id;
    int join;
    mutexlock_t *lock;
    int locked;

};

ucontext_t main_context;
void **returnValues;
struct Thread *threads;
int current_thread, recent_thread, old_thread, new_thread, numOfThreads, yieldNum;
int *yielded;

void threadManager(thFuncPtr funcPtr, void *argPtr) {

    void *returnValue = funcPtr(argPtr);
    returnValues = realloc(returnValues, (numOfThreads) * sizeof(void *));

    returnValues[current_thread] = returnValue;

    threads[current_thread].status = 1;

    for (int i = 0; i < numOfThreads; i++) {
        if (threads[i].join == current_thread) {
            swapcontext(&threads[current_thread].thread, &threads[i].thread);
        }
    }

    threadYield();
}

void threadInit(void) {
    threads = malloc(1 * sizeof(struct Thread));
    if (getcontext(&threads[0].thread) == -1) {
        perror("Failed to get context");
    }
    threads[0].status = 0;
    threads[0].id = 0;
    returnValues = calloc(1, sizeof(void *));
    yielded = malloc(1 * sizeof(int));
    numOfThreads = 1; // Number of threads
    old_thread = 0;
    new_thread = 0;
    recent_thread = 0; // Index of thread that called yield
    current_thread = 0; // Index of our working thread
    yieldNum = 0; // Number of threads that have yielded
    return;
}

int threadCreate(thFuncPtr funcPtr, void *argPtr) {
    old_thread = current_thread;
    current_thread = numOfThreads; // Do current thread first so i dont have to do numOfThreads - 1 for the index
    new_thread = current_thread;
    numOfThreads+=1;
    threads = realloc(threads, (numOfThreads) * sizeof(struct Thread));
    threads[current_thread].id = current_thread;
    threads[current_thread].join = -1;

    if (getcontext(&threads[current_thread].thread) == -1) {
        perror("Failed to get context");
    }

    threads[current_thread].status = 0;
    threads[current_thread].stack = malloc(STACK_SIZE);
    threads[current_thread].thread.uc_stack.ss_sp = threads[current_thread].stack;
	threads[current_thread].thread.uc_stack.ss_size = STACK_SIZE;

    makecontext(&threads[current_thread].thread, (void (*)(void))threadManager, 2, funcPtr, argPtr);

    swapcontext(&threads[old_thread].thread, &threads[current_thread].thread);

    return new_thread;
}

void threadYield(void) {
    int i = current_thread + 1;
    recent_thread = current_thread;

    while (i != recent_thread) {
        if (i == numOfThreads) {
            i = 0;
        }
        if (threads[i].status == 0 && i != recent_thread) {
            current_thread = i;
            break;
        }
        i++;
    }

    if (current_thread == recent_thread) {
        return;
    }
    else {
        swapcontext(&threads[recent_thread].thread, &threads[current_thread].thread);
    }
}

void threadJoin(int thread_id, void **result) {
    threads[current_thread].join = thread_id;
    if (thread_id >= numOfThreads) {
        return;
    }
    while (threads[thread_id].status != 1) {
        threadYield();
    }
    if (returnValues[thread_id] != NULL) {
            *result = returnValues[thread_id];
            return;
    }
    else {
        return;
    }
}

//exits the current thread -- closing the main thread, will terminate the program
void threadExit(void *result) {
    if (current_thread == 0) {
        exit(0);
    }
    else {
        threads[current_thread].status = 1;
        returnValues[current_thread] = result;
        threadYield();
        return;
    }
}

mutexlock_t * lockCreate(void) {
    mutexlock_t *lock = malloc(sizeof(mutexlock_t));
    lock->lock = 0;
    return lock;
}
void lockDestroy(mutexlock_t * lock) {
    free(lock);
    return;
}
void threadLock(mutexlock_t *lock) {
    while (lock != 0) {
        threadYield();
    }
    lock->lock = 1;
    threads[current_thread].lock = lock;

    return;
}

void threadUnlock(mutexlock_t *lock) {
    threads[current_thread].lock = NULL;
    lock->lock = 0;
    return;
}

//condition variable types and functions
struct condvar {
    int cond;
}; //opaque type -- you need to implement this in your source file
typedef struct condvar condvar_t;

condvar_t * condvarCreate(void) {
    return NULL;
}
void condvarDestroy(condvar_t * cv) {
    return;
}
void threadWait(mutexlock_t* lock, condvar_t *cv) {
    return;
}
void threadSignal(mutexlock_t* lock, condvar_t *cv) {
    return;
}

//this 
int interruptsAreDisabled;