#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../include/mthread.h"

void* excute(void* function, void** paras, size_t num) {
    size_t i = 0;
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * num);
    void* retval;
    // create threads
    for (i = 0; i < num; i++) {
        if (pthread_create(&threads[i], NULL, function, paras[i]))
            printf("Fail to create thread\n");
    }
    // join main
    for (i = 0; i < num; i++) {
        if (pthread_join(threads[i], &retval)) 
            printf("thread %lu cannot join, ret:%d", i, *(int*)retval);
    }
    free(threads);
}