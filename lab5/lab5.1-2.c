#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>

int global = 10;

int main (int argc, char* argv[])
{
    int local = 15;
    printf("local var address: %p content: %d\n", &local, local);
    printf("global var address: %p content: %d\n", &global,global);
    int ppid = getpid();
    printf("pid: %d\n",ppid);
    int pid = fork();

    if(pid == 0){
        printf("parent pid: %d\n", ppid);
        printf("child pid: %d\n", getpid());
        printf("child local var address: %p content: %d\n", &local, local);
        printf("child global var address: %p content: %d\n", &global,global);
        local = 7;
        global = 5;
        printf("child local: %d\n", local);
        printf("child global: %d\n", global);
        int chpid = fork();
        if(chpid == 0){
            printf("child child pid: %d\n", getpid());
            int cnt =0;
            while(cnt < 1){
                cnt++;
                sleep(1);
                exit(0);
            }    
        }
        else if(chpid == -1){
            perror("Error in ch process");
            return -1; 
        }
        else{
            //sleep(10);
            //kill(pid,SIGKILL);
            exit(5);
        }
    }
    else if(pid == -1){
        perror("error");
        return -1;
    }
    else{
        printf("parent local: %d global: %d\n", local, global);
        sleep(30);
        int st, err = 0;
        if(argc == 1)
            err = waitpid(pid,&st,0);
        if(err == -1){
            perror("wait error");
            return -1;
        }

        //if(WIFEXITED(st)){
            printf("child process exit with code %d\n", WEXITSTATUS(st));
       // }
    }

    return 0;
}
