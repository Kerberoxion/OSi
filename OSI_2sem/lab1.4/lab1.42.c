#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>

void clean(void* buf){
    free(buf);
    printf("clean\n");
}

void* thread_func(void* arg){
    char* str = malloc(3);
    str[0] = 'a';
    str[1] = 'b';
    str[2] = 'c';
    pthread_cleanup_push(clean, str);

    while(1){
        printf("%s\n", str);
    }
    pthread_cleanup_pop(0);
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

