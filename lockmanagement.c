#include <ucontext.h>
#include <stddef.h>
#include "mythreads.h"

//mutex lock types and functions.
struct mutexlock {
    int lock;
}; //opaque type -- you need to implement this in your source file
typedef struct mutexlock mutexlock_t;

mutexlock_t * lockCreate(void) {
    return NULL;
}
void lockDestroy(mutexlock_t * lock) {
    return;
}
void threadLock(mutexlock_t *lock) {
    return;
}

void threadUnlock(mutexlock_t *lock) {
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