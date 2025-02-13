#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

void* thread_func(void* arg){
    while(1){
        printf("Hello world!\n");
    }

    return NULL;
}

int main (void){
    pthread_t tid;
    int err;
    err = pthread_create(&tid, NULL, thread_func, NULL);
    if(err){
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    err = pthread_cancel(tid);
    if(err){
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
        return -1;
    }
    pthread_exit(NULL);
}

