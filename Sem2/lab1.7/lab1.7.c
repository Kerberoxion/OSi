#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>


#define STACK_SIZE 4096

#define MAX_THREADS 8

typedef struct uthread{
    void (*thread_func)(void*);
    void* arg;
    ucontext_t uctx;

}uthread_t;

uthread_t *uthreads[MAX_THREADS];
int uthread_count = 0;
int uthread_cur = 0;

void* create_stack(off_t size, char* file_name){
    void* stack;

    if(file_name){
        int stack_fd = open(file_name, O_RDWR | O_CREAT, 0660);
        if(stack_fd == -1){
            printf("open failed: %s\n", strerror(errno));
            exit(-1);
        }

        ftruncate(stack_fd, 0);
        ftruncate(stack_fd, size);

        stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_STACK, stack_fd, 0);
        if(stack == (void*)-1){
            printf("clone failed: %s\n", strerror(errno));
            close(stack_fd);
            exit(-1);
        
        close(stack_fd);    
        }
    }
    else{
        stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    }    
    
    memset(stack, 0x7f, size);

    return stack;

}

void start_thread(void){
    ucontext_t* ctx;

    for(int i=1; i< uthread_count; i++){
        ctx = &uthreads[i]->uctx;
        char* stack_from = ctx->uc_stack.ss_sp;
        char* stack_to = ctx->uc_stack.ss_sp + ctx->uc_stack.ss_size;

        if(stack_from <= (char*)&i && (char*)&i <= stack_to){
            uthreads[i]->thread_func(uthreads[i]->arg);
        }
    
    }
}


void uthread_create(uthread_t **ut, char* name, void (*thread_func)(void*), void* arg){
    char* stack;
    uthread_t *new_ut;
    int err;
    
    assert(name);

    stack = create_stack(STACK_SIZE, name);
    new_ut = (uthread_t*) (stack + STACK_SIZE - sizeof(uthread_t));

    err = getcontext(&new_ut->uctx);
    if(err == -1){
        printf("schedule: swapcontext() failed: %s\n", strerror(errno));
        exit(2);
    }
    
    new_ut->uctx.uc_stack.ss_sp = stack;
    new_ut->uctx.uc_stack.ss_size = STACK_SIZE - sizeof(uthread_t);
    new_ut->uctx.uc_link = NULL;

    makecontext(&new_ut->uctx, start_thread,0);
    new_ut->thread_func = thread_func;
    new_ut->arg = arg;
    //strncpy(new_ut->name, name, NAME_SIZE);
    uthreads[uthread_count] = new_ut;
    uthread_count++;
    *ut = new_ut;
}

void schedule(void){
    int err;
    ucontext_t *cur_ctx, *next_ctx;

    cur_ctx = &(uthreads[uthread_cur]->uctx);

    uthread_cur = (uthread_cur + 1) % uthread_count;

    next_ctx = &(uthreads[uthread_cur]->uctx);

    err = swapcontext(cur_ctx,next_ctx);
    if(err == -1){
        printf("swapcontext failed: %s\n", strerror(errno));
        exit(-1);
    }
}

void mythread1(void* arg){
    char* myarg = (char*)arg;
    printf("mythread1 started\n");

    for (int i = 0; i < 3; i++) {
        printf("%s\n", myarg);
        sleep(1);
        schedule();
    }
    printf("mythread1 finished\n");
}

int main(){
    uthread_t* ut[3];
    uthread_t main_thread;
    uthreads[0] = &main_thread;
    uthread_count = 1;

    printf("main started: %d\n", getpid());
    uthread_create(&ut[0], "thread1", mythread1,(void*) ("Hello from thread1"));
    uthread_create(&ut[1], "thread2", mythread1,(void*) ("Hello from thread2"));
    uthread_create(&ut[2], "thread3", mythread1,(void*) ("Hello from thread3"));

    while(1){
        schedule();
    }
    return 0;
}
