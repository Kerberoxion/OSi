#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

int main (void) {
    struct sockaddr_un sock;
    int clt_sock = socket(AF_UNIX,SOCK_STREAM,0);
    if(clt_sock == -1){
        perror("socket error: ");
        return -1;
    }
    
    sock.sun_family = AF_UNIX;
    strcpy(sock.sun_path,"./sock_file");
    int err = connect(clt_sock, (struct sockaddr *) &sock,sizeof(sock));
    if(err == -1){
        perror("connect error: ");
        close(clt_sock);
        return -1;
    }

    int value;
    while(1){
        err = read(clt_sock,&value, sizeof(value));
        if(err == 1){
            perror("read error: ");
            close(clt_sock);
            return -1;
        }
        err = write(clt_sock,&value,sizeof(value));
        if(err == 1){
            perror("read error: ");
            close(clt_sock);
            return -1;
        }

        printf("%d\n", value);
        sleep(1);
    }
    
    printf("Hello, World!");
    return 0;
}
