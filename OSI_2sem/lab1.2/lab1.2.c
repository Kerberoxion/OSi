#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

void* thread_func(void* arg){
    void* num = (void*)42;
    return num;
}

void* str_thread(void* arg){
    void* str = (void*) "hello world";
    return str;
}

int main (void){
    pthread_t tid;
    int err;
    int num;
    char* str;
    err = pthread_create(&tid, NULL, thread_func, NULL);
    if(err){
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_join(tid, (void*)&num);
    printf("number: %d\n", num);

    err = pthread_create(&tid, NULL, str_thread, NULL);
    if(err){
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    pthread_join(tid, (void*)&str);
    sleep(5);
    printf("%s\n", str);
    return 0;
}
