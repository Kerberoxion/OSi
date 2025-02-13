#include "lab1.6.h"

void* thread_func(void* arg){
    char* str = arg;
    printf("%s\n", str);
    str = "Hello, user!";
    arg = (void*)str;
}

int main(){
    mythread_t mythread;
    char* arg = "Hello world!";
    int err = mythread_create(&mythread,thread_func, (void*)arg);
    if(err){
        printf("error\n");
    }

    mythread_join(mythread, (void**)&arg);
    printf("%s\n", arg);
}
