#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* mythread(void* arg){
    printf("%d %ld\n", gettid(), pthread_self());
    //int err = pthread_detach(pthread_self());
    //if (err)
    //    printf("main: pthread_create() failed: %s\n", strerror(err));
    
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_attr_t thread_attr;
    int err;
    while(1){
        err = pthread_attr_init(&thread_attr);
        if(err != 0) {
            printf("Cannot create thread attribute: %s\n", strerror(err));
            return -1;
        }

        err =  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        if(err != 0) {
            printf("Cannot create thread attribute: %s\n", strerror(err));
            return -1;
        }

        err = pthread_create(&tid, &thread_attr, mythread, NULL);
            if (err) {
                printf("main: pthread_create() failed: %s\n", strerror(err));
                return -1;
            }

       pthread_attr_destroy(&thread_attr);     
       //sleep(1);
    }
    pthread_exit(NULL);
    return 0;
}

