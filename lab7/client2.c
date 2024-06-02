#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main (void) {
    struct sockaddr_in sock;
    int clt_sock = socket(AF_INET,SOCK_STREAM,0);
    if(clt_sock == -1){
        perror("socket error: ");
        return -1;
    }
    
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock.sin_port = htons(5555);

    int err = connect(clt_sock, (struct sockaddr *) &sock,sizeof(sock));
    if(err == -1){
        perror("connect error: ");
        close(clt_sock);
        return -1;
    }

    int value = 0;
    while(1){
        err = write(clt_sock,&value, sizeof(value));
        if(err == 1){
            perror("read error: ");
            close(clt_sock);
            return -1;
        }
        err = read(clt_sock,&value,sizeof(value));
        if(err == 1){
            perror("read error: ");
            close(clt_sock);
            return -1;
        }
        value++;
        printf("%d\n", value);
        sleep(1);
    }
    
    printf("Hello, World!");
    return 0;
}
