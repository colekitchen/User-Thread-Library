#include <ucontext.h>
#include <stddef.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdio.h>
#include <assert.h>
#include "mythreads.h"

//mutex lock types and functions.
struct mutexlock {
    int lock;
}; //opaque type -- you need to implement this in your source file
typedef struct mutexlock mutexlock_t;

//condition variable types and functions
struct condvar {
    int cond;
}; //opaque type -- you need to implement this in your source file
typedef struct condvar condvar_t;

struct Thread {
    ucontext_t *thread;
    mutexlock_t *lock;
    condvar_t *cv;
    void *stack;
    int status;
    int id;
    int join;
    int locked;
    int signaled;
    int order;

};

ucontext_t main_context;
void **returnValues;
struct Thread *threads;
int current_thread, recent_thread, old_thread, new_thread, numOfThreads, interruptsAreDisabled;

static void interruptDisable () {
    assert(!interruptsAreDisabled);
    interruptsAreDisabled = 1;
}

static void interruptEnable () {
    assert(interruptsAreDisabled);
    interruptsAreDisabled = 0;
}

void threadManager(thFuncPtr funcPtr, void *argPtr) {
    interruptEnable();
    void *returnValue = funcPtr(argPtr);
    interruptDisable();

    returnValues[current_thread] = returnValue;

    threads[current_thread].status = 1;


    /*for (int i = 0; i < numOfThreads; i++) {
        if (threads[i].join == current_thread) {
            swapcontext(threads[current_thread].thread, threads[i].thread);
            //break;
            //interruptEnable();
        }
    }*/

    /*for (int i = 0; i < numOfThreads; i++) {
        if (threads[i].status == 1 && i != current_thread) {
            free(threads[i].thread->uc_stack.ss_sp);
            threads[i].thread->uc_stack.ss_sp = NULL;
        }
    }*/
    interruptEnable();

    threadYield();
}

void threadInit(void) {
    interruptDisable();
    threads = malloc(1 * sizeof(struct Thread));
    threads[0].thread = malloc(sizeof(ucontext_t));
    if (getcontext(threads[0].thread) == -1) {
        perror("Failed to get context");
    }
    threads[0].status = 0;
    threads[0].id = 0;
    returnValues = calloc(1, sizeof(void *));
    numOfThreads = 1; // Number of threads
    old_thread = 0;
    new_thread = 0;
    recent_thread = 0; // Index of thread that called yield
    current_thread = 0; // Index of our working thread
    interruptEnable();
    return;
}

int threadCreate(thFuncPtr funcPtr, void *argPtr) {
    interruptDisable();
    old_thread = current_thread;
    current_thread = numOfThreads; // Do current thread first so i dont have to do numOfThreads - 1 for the index
    new_thread = current_thread;
    numOfThreads+=1;
    threads = realloc(threads, (numOfThreads) * sizeof(struct Thread));
    threads[current_thread].thread = malloc(sizeof(ucontext_t));
    returnValues = realloc(returnValues, (numOfThreads) * sizeof(void *));
    returnValues[current_thread] = NULL;
    threads[current_thread].id = current_thread;
    threads[current_thread].join = -1;
    //threads[current_thread].join = -1;

    if (getcontext(threads[current_thread].thread) == -1) {
        perror("Failed to get context");
    }

    threads[current_thread].status = 0;
    //threads[current_thread].stack = malloc(STACK_SIZE);
    threads[current_thread].thread->uc_stack.ss_sp = malloc(STACK_SIZE);;
	threads[current_thread].thread->uc_stack.ss_size = STACK_SIZE;

    makecontext(threads[current_thread].thread, (void (*)(void))threadManager, 2, funcPtr, argPtr);

    swapcontext(threads[old_thread].thread, threads[current_thread].thread);
    interruptEnable();

    return new_thread;
}

void threadYield(void) {
    interruptDisable();
    int i = current_thread + 1;
    recent_thread = current_thread;

    while (i != recent_thread) {
        if (i == numOfThreads) {
            i = 0;
            if (recent_thread == 0) {
                break;
            }
        }
        if (threads[i].status == 0 && i != recent_thread) {
            current_thread = i;
            break;
        }
        i++;
    }

    for (int i = 0; i < numOfThreads; i++) {
        if (threads[recent_thread].status == 1 && threads[i].join == recent_thread) {
            current_thread = i;
        }
    }

    //printf("Going to thread %d\n", current_thread);
    if (current_thread != recent_thread) {
        swapcontext(threads[recent_thread].thread, threads[current_thread].thread);
    }

    interruptEnable();
    return;
}

void threadJoin(int thread_id, void **result) {
    interruptDisable();
    if (thread_id >= numOfThreads) {
        interruptEnable();
        return;
    }
    threads[current_thread].join = thread_id;
    while (threads[thread_id].status != 1) {
        interruptEnable();
        threadYield();
        interruptDisable();
    }
    
    threads[current_thread].join = -1;

    if (returnValues[thread_id] != NULL) {
            *result = returnValues[thread_id];
            free(threads[thread_id].thread->uc_stack.ss_sp);
            //free(returnValues[thread_id]);
            //threads[thread_id].thread->uc_stack.ss_sp = NULL;
            //free(threads[thread_id].thread);
            //threads[thread_id].thread = NULL;
            //threads[thread_id].stack = NULL;
            interruptEnable();
            return;
    }
    else {
        free(threads[thread_id].thread->uc_stack.ss_sp);
        //free(returnValues[thread_id]);
        //threads[thread_id].thread->uc_stack.ss_sp = NULL;
        //free(threads[thread_id].thread);
        //threads[thread_id].thread = NULL;
        //threads[thread_id].stack = NULL;
        interruptEnable();
        return;
    }
}

//exits the current thread -- closing the main thread, will terminate the program
void threadExit(void *result) {
    interruptDisable();
    if (current_thread == 0) {
        exit(0);
    }
    else {
        threads[current_thread].status = 1;
        returnValues[current_thread] = result;
        swapcontext(threads[current_thread].thread, threads[0].thread);
    }
}

mutexlock_t * lockCreate(void) {
    interruptDisable();
    mutexlock_t *lock = malloc(sizeof(mutexlock_t));
    lock->lock = 0;
    interruptEnable();
    return lock;
}
void lockDestroy(mutexlock_t * lock) {
    interruptDisable();
    free(lock);
    interruptEnable();
    return;
}
void threadLock(mutexlock_t *lock) {
    interruptDisable();
    while (lock->lock != 0) {
        interruptEnable();
        threadYield();
        interruptDisable();
    }
    lock->lock = 1;
    threads[current_thread].lock = lock;
    interruptEnable();

    return;
}

void threadUnlock(mutexlock_t *lock) {
    interruptDisable();
    lock->lock = 0;
    threads[current_thread].lock = NULL;
    interruptEnable();
    return;
}

condvar_t * condvarCreate(void) {
    interruptDisable();
    condvar_t *condvar = malloc(sizeof(condvar_t));
    condvar->cond = 0;
    interruptEnable();
    return condvar;
}
void condvarDestroy(condvar_t * cv) {
    interruptDisable();
    free(cv);
    interruptEnable();
    return;
}
void threadWait(mutexlock_t* lock, condvar_t *cv) {
    interruptDisable();
    int max = -1;
    if (threads[current_thread].lock != lock) {
        perror("Thread waiting on lock it doesn't own\n");
        exit(0);
    }

    interruptEnable();
    threadUnlock(lock);
    interruptDisable();

    threads[current_thread].signaled = 0;
    threads[current_thread].cv = cv;
    threads[current_thread].lock = lock;
    threads[current_thread].order = 0;

    for (int i = 0; i < numOfThreads; i++) {
        if (threads[i].lock == lock && threads[i].cv == cv && threads[i].order != -1 && i != current_thread) {
            if (threads[i].order > max) {
                max = i;
            }
        }
    }

    threads[current_thread].order = max + 1;

    while (threads[current_thread].signaled != 1) {
        interruptEnable();
        threadYield();
        interruptDisable();
    }

    interruptEnable();
    threadLock(lock);
    interruptDisable();
    threads[current_thread].cv = NULL;
    threads[current_thread].order = -1;
    interruptEnable();
    return;
}
void threadSignal(mutexlock_t* lock, condvar_t *cv) {
    interruptDisable();
    int min = 1000000000;
    for (int i = 0; i < numOfThreads; i++) {
        if (threads[i].lock == lock && threads[i].cv == cv && threads[i].signaled == 0) {
            if (threads[i].order > -1 && threads[i].order < min) {
                min = i;
            }
            threads[min].signaled = 1;
            //interruptEnable();
            swapcontext(threads[current_thread].thread, threads[min].thread);
        }
    }
    interruptEnable();
    return;
}
