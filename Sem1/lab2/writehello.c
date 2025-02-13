#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
void printHello(){
    syscall(SYS_write,0,"hello world\n",12);
}

int main(){

   // write(1,"hello world\n",12);
    printHello();
}
