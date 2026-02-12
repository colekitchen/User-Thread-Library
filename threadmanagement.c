#include <ucontext.h>
#include <stddef.h>
#include "mythreads.h"

void threadInit(void) {
    return;
}
int threadCreate(thFuncPtr funcPtr, void *argPtr) {
    return 0;
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