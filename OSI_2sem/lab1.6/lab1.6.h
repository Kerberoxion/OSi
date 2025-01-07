#pragma once
#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <signal.h>

#define PAGE 4096
#define STACK_SIZE 3*PAGE
typedef void* (*start_routine_t) (void*);
typedef struct _mythread{
    start_routine_t start_routine;
    void* arg;
    void* retval;
    volatile int finished;
    volatile int joined;
} mythread_struct_t;

typedef mythread_struct_t* mythread_t;
void* create_stack(off_t size);
int thread_startup(void* arg);
int mythread_create(mythread_t* mytid, void* (start_routine) (void*), void* arg);
void mythread_join(mythread_t mytid, void **retval);
