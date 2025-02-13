#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
int flag = 0;

void stack(){
    int a[10 * 4096];
    printf("work\n");
    sleep(1);
    stack();
}

void heap(){
    int* a;
    for(int i = 0; i < 10; i++ ){
        a = malloc(100*4096);
        sleep(1);
    }
    free(a);
}

void func(){
    //printf("SIGSEGV XAXAXAXXAXAXAXAXAXAXAXXAXAXAXAXAXAXXAXAXAXAXAXXAXA\n");
    flag = 1;
    return;
}

void m_map(char* argv[]){
    char* p = mmap(NULL, 10*4096, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    if(p == -1){
        perror("Error while mmap");
        return;
    }
    //sleep(10);
    int err;
    if(strcmp(argv[2],"none") == 0)
        err = mprotect(p, 10*4096,PROT_NONE);
    
    else if(strcmp(argv[2],"w") == 0)
        err = mprotect(p, 10*4096, PROT_WRITE);
    
    else if(strcmp(argv[2], "r") == 0)
        err = mprotect(p,10*4096, PROT_READ);

    else if(strcmp(argv[2],"wr") == 0)
        err = mprotect(p,10*4096, PROT_READ|PROT_WRITE);

    if(err == -1){
        perror("error while mprotect");
        return;
    }
    //sleep(10);
    signal(SIGSEGV,func);
    printf("%s\n", p);
   // char* a;
   // a = p + 1;
   // *a+=1;  
   // printf("%s %s\n", a, p);
    err = munmap(p+4*4096, 3*4096);
    if(err == -1){
        perror("Error while unmap");
    }
    sleep(15);

}

int main (int argc, char* argv[])
{
    int pid = getpid();
    printf("%d\n", pid);
   // sleep(15);
   if(strcmp(argv[1], "stack") == 0)
       stack();
   else if(strcmp(argv[1],"heap") ==0)
       heap();
   else if(strcmp(argv[1], "mmap") == 0)
       m_map(argv);
   else{
    printf("you don't write command line arguments\n");
   }

   printf("AAAAAAAAAAAAA");
  return 0;
}
