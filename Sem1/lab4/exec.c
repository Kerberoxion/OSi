#include <stdio.h>
#include <unistd.h>

void exec(char* argv[]){
    int pid = getpid();
    printf("%d\n", pid);
    sleep(1);
    if(execv(argv[0],argv) == -1){
        perror("Error");
        return;
    }    
    printf("Hello World!\n");
}

int main (int argc, char* argv[]){
    exec(argv);
    return 0;
}
