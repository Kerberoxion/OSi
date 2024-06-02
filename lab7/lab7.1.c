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
    server_sock = socket(AF_INET,SOCK_DGRAM,0);
    if(server_sock == -1){
        perror("Error");
        return -1;
    }
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = INADDR_ANY;
    sock.sin_port = htons(5555);
    int err = bind(server_sock, (struct sockaddr *) &sock, sizeof(sock));
    if(err == -1){
        perror("bind failed: ");
        close(server_sock);
        return -1;
    }
    

    char s_msg[] = "MESSAGE";
    client.sin_family = AF_INET;
    len = sizeof(client);
    
    printf("waiting connections\n");
    while(1){
        char msg[1024];
        err = recvfrom(server_sock,msg,8,0, (struct sockaddr *)&client,&len);
        if(err == -1){
            perror("recv error: ");
            break;
        }

        printf("%s\n",msg);

        err = sendto(server_sock,s_msg,8, 0, (struct sockaddr *)&client, sizeof(client));
        if(err == -1){
            perror("send error: ");
            break;
        }
        sleep(1);
    }
    close(clt_sock);
    close(server_sock);
    return 0;
}
