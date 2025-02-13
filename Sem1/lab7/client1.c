#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

int main (void){
    struct sockaddr_in sock;
    struct sockaddr_in client;
    int server_sock, clt_sock, len = 0;
    clt_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(clt_sock == -1){
        perror("Error");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock.sin_port = htons(5555);
    

    char msg[8] = "message";
    len = sizeof(sock);
    while(1){
        char buf[100];
        int err = sendto(clt_sock, msg, 8, 0,(struct sockaddr *) &sock, len);
        if(err == -1){
            perror("send error: ");
            break;
        }
        msg[4]++;
       
        err = recvfrom(clt_sock,buf,8,0,(struct sockaddr *)&sock,&len);
        if(err == -1){
            perror("recv error: ");
            break;
        }
        printf("%s\n", buf);
    }
    close(clt_sock);
    return 0;
}
