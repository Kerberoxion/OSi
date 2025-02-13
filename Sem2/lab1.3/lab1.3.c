#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

typedef struct person{
    int age;
    char* name;
} person;

void* mythread(void* arg){
    person* Mini_Person = (person*)arg;
    printf("%d, %s\n", Mini_Person->age, Mini_Person->name);
    free(Mini_Person->name);
    free(Mini_Person);
    return NULL;
}


int main() {
    person* Person = (person*) malloc(sizeof(person));
    Person->age = 45;
    Person->name = (char*)malloc(strlen("Vlad")+1);
    strcpy(Person->name, "Vlad");
    pthread_t tid;
    int err;
    pthread_attr_t thread_attr;
        err = pthread_attr_init(&thread_attr);
        if(err != 0) {
            printf("Cannot create thread attribute: %s\n", strerror(err));
            return -1;
        }

        err =  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        if(err != 0) {
            printf("Cannot create thread attribute: %s\n", strerror(err));
            return -1;
        }

    err = pthread_create(&tid,&thread_attr, mythread, (void*)Person);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    pthread_attr_destroy(&thread_attr);
    pthread_exit(NULL);
}
