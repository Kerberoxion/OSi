#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (void){

    int pipefd[2];
    int err = pipe(pipefd);
    if(err == -1){
        perror("Error while pipe");
        return -1;
    }

    int alert = 0;
    int pid = fork();
    if(pid == -1){
        perror("Error");
        return -1;
    }
    else if(pid == 0){
        int num = 0;
        int cnt = 0;
        while(!alert){
            for(int i=0; i<4096/4;i++){
                err = read(pipefd[0],&num,4);
                if(err == -1){
                    perror("Error while reading: ");
                    return -1;
                }
                if(num != cnt){
                    printf("alert! %d\n", num);
                    alert = 1;
                }
                else{
                    printf("%d ",num);
                }
                cnt++;
                if(i == 4096/4 -1)
                    cnt = 0;
            }
        }
        exit(0);
    }
    else{
        int count = 0;
        while(1){
            for(int i=0; i<4096/4; i++){
                err = write(pipefd[1],&count,4);
                if(err == -1){
                    perror("Error while writing: ");
                    return -1;
                }
                count++;
                if(i == 4096/4 - 1)
                    count = 0;
            }
        }
    }
    int st;
    waitpid(pid, &st,0);
    return 0;
}

