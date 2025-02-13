#include "lab1.6.h"

void* create_stack(off_t size){
    char stack_file[128];
    int stack_fd;
    void* stack;
    snprintf(stack_file, sizeof(stack_file), "stack");
    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    if(stack_fd == -1){
        printf("open failed: %s\n", strerror(errno));
        exit(-1);
    }

    ftruncate(stack_fd, 0);
    ftruncate(stack_fd, size);

    stack = mmap(NULL, size, PROT_NONE, MAP_SHARED, stack_fd, 0);
    if(stack == (void*)-1){
        printf("clone failed: %s\n", strerror(errno));
        close(stack_fd);
        exit(-1);

    }
    close(stack_fd);

    return stack;

}

int thread_startup(void* arg){
    mythread_t tid = (mythread_t)arg;
    mythread_struct_t *thread = tid;

    thread->retval = thread->start_routine(thread->arg);
    thread->finished = 1;
    while(!thread->joined)
        sleep(1);
    
}

int mythread_create(mythread_t* mytid, void* (start_routine) (void*), void* arg){
    mythread_struct_t* thread;

    void* child_stack = create_stack(STACK_SIZE);
    mprotect(child_stack + PAGE, STACK_SIZE - PAGE, PROT_READ | PROT_WRITE);
    memset(child_stack + PAGE, 0x7f, STACK_SIZE - PAGE);

    thread = (mythread_struct_t*) (child_stack + STACK_SIZE - sizeof(mythread_struct_t));
    thread->arg = arg;
    thread->finished = 0;
    thread->joined = 0;
    thread->start_routine = start_routine;
    child_stack = (void*)thread;

    int child_pid = clone(thread_startup, child_stack, CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | CLONE_FS | SIGCHLD,(void*)thread);
    if(child_pid == -1){
        printf("clone failed: %s\n", strerror(errno));
        return -1;
    }
    *mytid = thread;
    return 0;
}

void mythread_join(mythread_t mytid, void **retval){
    mythread_struct_t* thread = mytid;

    while(!thread->finished){
        usleep(1);
    }
    *retval = thread->retval;
    thread->joined = 1;

}
