#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdatomic.h>

typedef struct {
    int lock;
} spinlock_t;

void spinlock_init(spinlock_t *s){
    s->lock = 1;
}

void spinlock_lock(spinlock_t *s){
    while(1){
        int one = 1;
        if(atomic_compare_exchange_strong(&s->lock, &one, 0))
            break;
    }
}

void spinlock_unlock(spinlock_t *s){
    int zero = 0;
    atomic_compare_exchange_strong(&s->lock, &zero, 1);
}

spinlock_t lock;
volatile int var = 0;

void* pthread1(void* arg){
    for(int i = 0; i < 100000; i++){
        spinlock_lock(&lock);
        var++;
        printf("var: %d\n", var);
        spinlock_unlock(&lock);
    }
}

void* pthread2(void* arg){
    for(int i = 0; i<100000; i++){
        spinlock_lock(&lock);
        var++;
        printf("var: %d\n", var);
        spinlock_unlock(&lock);
    }
}

int main (){
    pthread_t th1, th2;
    spinlock_init(&lock);
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
