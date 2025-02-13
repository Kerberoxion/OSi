#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global = 5;
pthread_t tid;

void *mythread(void *arg) {
    printf("///////////////////////////////////////////////////////////////////////");
    int local = 2;
    int static s_local = 3;
    int const c_local = 4;

	printf("mythread [pid: %d ppid: %d tid: %d threadself:%ld]: Hello from mythread!\n", getpid(), getppid(), gettid(), pthread_self());
	printf("global address %p, local address %p, local static address %p, local const address %p\n", &global, &local, &s_local, &c_local);

    printf("global %d, local %d \n", global, local);
    local-=1;
    global+=1;
    printf("new global %d, new local %d \n", global, local);

    if(pthread_equal(tid, pthread_self()))
        printf("id thread are equal\n\n");
    else
        printf("id threads are not equal\n\n");
        
    return NULL;
}

int main() {
	int err;
	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());
    
    for(int i = 0; i < 1; i++){
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	    err = pthread_create(&tid, NULL, mythread, NULL);
	    if (err) {
	        printf("main: pthread_create() failed: %s\n", strerror(err));
		    return -1;
	    }
        pthread_join(tid,NULL);
    }
    pthread_exit(NULL);
}

