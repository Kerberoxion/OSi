#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int uninit_g;
int global = 5;
const int c_global = 8;

void heap(){
    char* buf1 = malloc(50);
    strcpy(buf1,"hello world!");
    printf("%s\n", buf1);
    free(buf1);
    printf("%s\n", buf1);
    
    char* buf2 = malloc(50);
    strcpy(buf2, "HELLO WORLD!!");
    printf("%s\n", buf2);
    buf2 = buf2 + 25;
    free(buf2);
    printf("%s\n", buf2);
}

int* func(){
    int a = 5;
    return &a;
}

int main (int argc, char* argv[])
{
    int local = 5; 
    static int s_local = 1;
    const int c_local = 3;
    printf("uninit global %p\n", &uninit_g);
    printf("global: %p\n", &global);
    printf("const global: %p\n", &c_global);
    printf("local: %p\n", &local);
    printf("const local: %p\n", &c_local);
    printf("static local: %p\n", &s_local);
    
    printf("address in func: %p\n", func());
    //heap();
    
    if(argc > 1 && strcmp(argv[1],"pid") == 0){
        int p = getpid();
        printf("pid: %d\n", p);
        sleep(6000);
    }

    char* env = getenv("MYVAR");
    if(env == NULL){
        printf("there are no env variable");
        return 0;
    }
    printf("MYVAR is %s\n", env);
    if(setenv("MYVAR","no",1) == -1){
        perror("Error");
    }

    printf("now MYVAR is %s\n", getenv("MYVAR"));

    return 0;
}
