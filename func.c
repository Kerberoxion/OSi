#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

void makeDir(char* name){
    if (mkdir(name, 0777) == 0) {
        printf("Folder created\n");
    }
    else if(errno == EEXIST){
        perror("mkdir rror");
    }
    else {
        perror("mkdir error");
    }

}

void readDir(char* name){

    DIR *dir;
    struct dirent *ep;
    dir = opendir (name);
    if (dir != NULL) {
        while (ep = readdir (dir)) {
            printf("%s\n",ep->d_name);
        }
        closedir (dir);
    }
    else {
        printf("Couldn't open the directory\n");
    }

}

void removeDir(char* name){
    if(rmdir(name) == 0){
        printf("folder deleted succesfully\n");
    }
    else{
        perror("Error");
    }
}

void readfile(char* name){
    FILE* file = fopen(name,"r");
    if(!file){
        printf("Error occured while opening file\n");
        return;
    }

    if(fseek(file,0,SEEK_END)){
        perror("fseek_end error");
        fclose(file);
        return;
    }

    long size = ftell(file);
    if(size == -1){
        perror("ftell error");
        fclose(file);
        return;
    }

    if(fseek(file,0,SEEK_SET)){
        perror("fseek_set error");
        fclose(file);
        return;
    }

    char* str = malloc(size * sizeof(char));
    fread(str,sizeof(char),size,file);
    if(ferror(file))
        perror("fread error");
    printf("%s\n", str);
    fclose(file);
    free(str);
}

void rmfile(char* name){
    if(remove(name)){
        perror("remove error");
        return;
    }
    printf("file succesfully removed\n");
}

void makefile(char* name){
    FILE* file = fopen(name,"w");
    if(!file){
        printf("Error occured while opening file\n");
        return;
    }
    else{
        printf("File succesfully created\n");
    }
    fclose(file);
}

void createLink(char* name){
    if(symlink(name, "link")){
        perror("symlink error");
    }
    printf("Symlink succesfully created\n");
}

void readSymlink(char* name){
    int size = 100;
    char* buffer = malloc(size * sizeof(char));
    int count = readlink(name,buffer,size);
    if(count == -1){
        perror("raedlink error");
        return;
    }
    printf("%s \n",buffer);

    free(buffer);
}

void readInLink(char* name){
    FILE* file = fopen(name,"r");
    if(!file){
        printf("Error occured while opening file\n");
        return;
    }

    if(fseek(file,0,SEEK_END)){
        perror("fseek_end link error");
        fclose(file);
        return;
    }

    long size = ftell(file);
    if(size == -1){
        perror("ftell link error");
        fclose(file);
        return;
    }
    if(fseek(file,0,SEEK_SET)){
        perror("fseek_set link error");
        fclose(file);
        return;
    }

    char* str = malloc(size * sizeof(char));
    fread(str,sizeof(char),size,file);
    if(ferror(file))
        perror("fread link error");
    printf("%s\n", str);    
    fclose(file);
    free(str);

}

void hardlink(char* name){
    if(link(name,"hardlink_file")){
        perror("hardlink error");
        return;
    }
    printf("Hardlink created succesfully\n");
}

void printMode(char* name){

    int fd = open(name, O_RDONLY);
    if(fd == -1){
        perror("open error");
        return;
    }
    struct stat st;
    if(fstat(fd, &st)){
        perror("Stat error");
        return;
    }
    printf("file has: %ld hard links\n", st.st_nlink);
    printf("File Permissions: ");
    printf( (st.st_mode & S_IRUSR) ? "r" : "-");
    printf( (st.st_mode & S_IWUSR) ? "w" : "-");
    printf( (st.st_mode & S_IXUSR) ? "x" : "-");
    printf( (st.st_mode & S_IRGRP) ? "r" : "-");
    printf( (st.st_mode & S_IWGRP) ? "w" : "-");
    printf( (st.st_mode & S_IXGRP) ? "x" : "-");
    printf( (st.st_mode & S_IROTH) ? "r" : "-");
    printf( (st.st_mode & S_IWOTH) ? "w" : "-");
    printf( (st.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
    close(fd);
}

void changemode(char* name){
    if(chmod(name, 0777)){
        perror("chmod error");
        return;
    }
    printf("file mode succesfully change\n");
}

