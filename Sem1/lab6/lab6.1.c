#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void){
    unsigned int* addr;
    void* p = mmap(NULL,4096,PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED|MAP_ANONYMOUS, 0,0);
    if(p == -1){
        perror("Error while mmap");
        return -1;
    }
    addr = p;
    int alert = 0;
    printf("pid: %d\n", getpid());
    int pid = fork();
    if(pid == -1){
        perror("Error");
        munmap(p,4096);
        return -1;
    }
    else if(pid == 0){
        int cnt = 0;
        while(!alert){
            for(int i=0; i<4096/4;i++){
                if(addr[i] != cnt){
                    printf("alert! %d\n", addr[i]);
                    alert = 1;
                }
                else{
                    printf("%d ",addr[i]);
                }
                cnt++;
            }
        }
        exit(0);
    }
    else{
       // *addr = 4096;
        printf("num %d\n", *addr);
        int count = 0;
        while(!alert){
            for(int i=0; i<4096/4; i++){
                addr[i] = count;
                count++;
            }
        }
    }
    printf ("Hello, World!\n");
    int st;
    waitpid(pid, &st,0);
    munmap(p,4096);
    return 0;
}
