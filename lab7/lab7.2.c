#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

int main (void) {
    struct sockaddr_in sock;
    struct sockaddr_in client;
    int server_sock, clt_sock, len = 0;
    server_sock = socket(AF_INET,SOCK_STREAM,0);
    if(server_sock == -1){
        perror("Error");
        return -1;
    } 

    signal(SIGPIPE,SIG_IGN);

    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = INADDR_ANY;
    sock.sin_port = htons(5555);


    int err = bind(server_sock, (struct sockaddr *) &sock, sizeof(sock));
    if(err == -1){
        perror("bind failed: ");
        close(server_sock);
        return -1;
    }
    err = listen(server_sock,5);
    if(err == -1){
        perror("listen error: ");
        close(server_sock);
        return -1;
    }
    
    printf("Waiting connections\n");
    
    while(1){

        clt_sock = accept(server_sock, (struct sockaddr *) &client,&len);
        if(clt_sock == -1){
            perror("accept failed: ");
            close(server_sock);
            return -1;
        }

        int pid = fork();
        if(pid == -1){
            perror("fork failed");
            close(clt_sock);
            close(server_sock);
            return 1;
        }
        else if(pid == 0)
            break;
        close(clt_sock);
    }
    int value = 0;
    while(1){
        err = read(clt_sock,&value,sizeof(value));
        if(err == -1){
            perror("write failed: ");
            break;
        }

        err = write(clt_sock,&value,sizeof(value));
        if(err == -1){
            perror("write failed: ");
            break;
        }

        printf("%d\n", value);
        sleep(1);
    }    
    close(clt_sock);
    close(server_sock);
    printf("Hello, World!");
    return 0;
}
