#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>

int main (int argc, char* argv[])
{
    FILE* file = fopen(argv[1],"r");
    if(ferror(file)){
        perror("fopen error: ");
    }
    int err = fseek(file,0,SEEK_END);
    if(err == -1){
        perror("error: ");
    }

    int length = ftell(file);
    if(length == -1){
        perror("error: ");
    }
    err = fseek(file,0,SEEK_SET);
    if(err == -1){
        perror("error: ");
    }

    char *buf = malloc(length * sizeof(char));
    err = fread(buf,sizeof(char),length,file);
    if(ferror(file)){
        perror("fread error: ");
    }
    fclose(file);
    printf("%s", buf);
    printf("real uid: %d, euid: %d\n", getuid(), geteuid());
    return 0;
}
