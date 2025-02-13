#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

typedef struct {
    int lock;
} mutex_t;

void mutex_init(mutex_t *m){
    m->lock = 1;
}

static int futex(int* uaddr, int futex_op, int val, const struct timespec *timeout, 
   int* uaddr2, int val3){
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr, val3);
}

void mutex_lock(mutex_t *m){
    int err;

    while(1){
        int one = 1;

        if(atomic_compare_exchange_strong(&m->lock, &one, 0))
            break;
    
        err = futex(&m->lock, FUTEX_WAIT, 0,NULL,NULL,0);
        if(err == -1 && errno != EAGAIN){
            printf("futex FUTEX_WAIT failed: %s\n", strerror(errno));
            abort();
        }
    }

}

void mutex_unlock(mutex_t* m){
    const int zero = 0;
    if(atomic_compare_exchange_strong(&m->lock, &zero, 1)){
        int err;

        err = futex(&m->lock, FUTEX_WAKE, 1,NULL,NULL,0);
        if(err == -1){
            printf("futex FUTEX_WAKE failed: %s\n", strerror(errno));
            abort();
        }

    }

}

mutex_t lock;
volatile int var = 0;

void* pthread1(void* arg){
    for(int i = 0; i < 100000; i++){
        mutex_lock(&lock);
        var++;
        printf("var: %d\n", var);
        mutex_unlock(&lock);
    }
}

void* pthread2(void* arg){
    for(int i = 0; i<100000; i++){
        mutex_lock(&lock);
        var++;
        printf("var: %d\n", var);
        mutex_unlock(&lock);
    }
}

int main (){
    pthread_t th1, th2;
    mutex_init(&lock);
    int err;

    err = pthread_create(&th1, NULL, pthread1, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&th2, NULL, pthread2, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }


    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    return 0;
}

