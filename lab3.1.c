#include <stdio.h> 
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

int createDir(char path[], char* name){
    int end = 0;
    for(int i = strlen(path) - 1; i >= end; i--){
        if(path[i] == '/')
            end = i;
        else{
            name[strlen(path) - 1 - i] = path[i];
        }
        name[strlen(path) - end] = '\0';
    }

    if (mkdir(name, 0777) == 0) {
        printf("Folder created\n");
    }
    else if(errno == EEXIST){
        perror("Error");
        return 0;
    }
    else {
        perror("Error");
        return 1;
    }
    return 0;
}

void func(char* old,const char* buf, char* name){
    char new[100] = "";
    char cpath[100] = "./";
    strcat(cpath,name);
    int N_old = strlen(old);
    for(int i= N_old - 1; i>=0; i--){
        new[N_old - 1 - i] = old[i];
    }
    new[N_old] = '\0';
    char path[100] = "";
    strcat(path, buf);
    char* tmp = "/";
    strcat(path, tmp);
    char* oldpath = path;
    strcat(oldpath,old);

    FILE* file = fopen(oldpath,"r");
    if(!file){
        printf("Error occured while opening file\n");
        return;
    }

    if(fseek(file,0,SEEK_END)){
        perror("error");
        fclose(file);
        return;
    }

    long size = ftell(file);
    if(size == -1){
        perror("error");
        fclose(file);
        return;
    }

    if(fseek(file,0,SEEK_SET)){
        perror("error");
        fclose(file);
        return;
    }

    char* str = malloc(size * sizeof(char));
    fread(str,sizeof(char),size,file);
    if(ferror(file))
        perror("error");
    fclose(file);
    
    char npath[100] = "";
    strcat(npath, cpath);
    tmp = "/";
    strcat(npath, tmp);
    char newpath[100] = "";
    strcat(newpath,npath);
    strcat(newpath,new);

    FILE* newfile = fopen(newpath,"w");
    if(!newfile){
        printf("Error occured while opening file\n");
        return;
    }
    for(int i=strlen(str) - 1; i>=0; i--){
        fputc(str[i],newfile);
    }

    
    fclose(newfile);
    free(str);
}

int main(int argc, char* argv[]) {
    char name[100] = "";
    char buf[100] = "";
    strcat(buf,argv[1]);
    if(createDir(buf,name))
        return 0;

    DIR *dir;
    struct dirent *ep;
    dir = opendir (argv[1]);
    if (dir != NULL) {
        while (ep = readdir (dir)) {
            if(ep->d_type == DT_REG){
                
                func(ep->d_name,buf, name);
            }   
        }
        closedir (dir);
    }
    else {
        printf("Couldn't open the directory.");
    }
    return 0; 
}
