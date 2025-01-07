#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


volatile int flag = 0;
void my_sigaction(){
    flag = 1;
}

void* thread1_func(void* arg){
    sigset_t sigset;
    sigfillset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset,NULL);
    /*int err = pthread_sigmask(SIG_SETMASK, &sigset, NULL);
    if(err){
        printf("main3: pthread_create() failed: %s\n", strerror(err));
        exit(-1);
    }*/
    printf("thread1 [pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());

    while(1){
        sleep(1);
    }
    return NULL;
}

void* thread2_func(void* arg){
    printf("thread2 [pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());
    sigset_t sigset;
    sigaddset(&sigset, SIGINT);
    //sigprocmask(SIG_SETMASK, &sigset, NULL);
    int err = pthread_sigmask(SIG_SETMASK, &sigset, NULL);
    if(err){
        printf("main3: pthread_create() failed: %s\n", strerror(err));
        exit(-1);
    }

    while(1){
        int err;
        struct sigaction sigact;
        sigact.sa_flags = 0;
        sigact.sa_sigaction = my_sigaction;

        err = sigaction(SIGINT,&sigact, NULL);
        if(err){
            printf("sigaction failed: %s\n", strerror(err));
            return NULL;
        }

        if(flag){
            printf("SIGINT\n");
            flag = 0;
        }
    }
    return NULL;
}

void* thread3_func(void* arg){
    printf("thread3 [pid:%d ppid:%d tid:%d]\n", getpid(), getppid(), gettid());
    int err, sig;
    sigset_t sigset;
    sigaddset(&sigset, SIGQUIT);

    while(1){
        err = sigwait(&sigset, &sig);
        printf("SIGQUIT signal %d\n", sig);
        if(err){
            printf("sigwait failed: %s\n", strerror(err));
            exit(-1);
        }
    }
    return NULL;
}

int main () {
    pthread_t tid1;
    int err;
    printf("main pid: %d\n", getpid());

    //sigset_t sigset;
    //sigaddset(&sigset, SIGQUIT);
    //sigprocmask(SIG_SETMASK, &sigset, NULL);


    err = pthread_create(&tid1, NULL, thread1_func, NULL);
    if(err){
        printf("main1: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    pthread_t tid2;
    err = pthread_create(&tid2, NULL, thread2_func, NULL);
    if(err){
        printf("main2: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    pthread_t tid3;
    err = pthread_create(&tid3, NULL, thread3_func, NULL);
    if(err){
        printf("main3: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }
    sigset_t sigset;
    sigfillset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset,NULL);

    err = pthread_join(tid1, NULL);
    if(err){
        printf("main1: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid2, NULL);
    if(err){
        printf("main2: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid3, NULL);
    if(err){
        printf("main3: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }


}
