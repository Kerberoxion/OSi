#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>

#define CLIENTS_COUNT 10

int main (void) {

    struct sockaddr_in sock;
    struct sockaddr_in client;
    int server_sock, len = 0;
    int clt_socks[CLIENTS_COUNT];
    int s = 0;

    signal(SIGPIPE,SIG_IGN);
    memset(clt_socks, -1, sizeof(clt_socks));

    server_sock = socket(AF_INET,SOCK_STREAM,0);
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
    err = listen(server_sock,5);
    if(err == -1){
        perror("listen error: ");
        close(server_sock);
        return -1;
    }
    
    printf("Waiting connections\n");
    int value = 0;
    while(1){
        fd_set rfds;
        struct timeval tv;
        int max_fd;
        
        FD_ZERO(&rfds);
        FD_SET(server_sock, &rfds);
        max_fd = server_sock;

        for(int i = 0; i < CLIENTS_COUNT; i++){
            if(clt_socks[i] != -1){
                printf("add %d\n", clt_socks[i]);
                FD_SET(clt_socks[i], &rfds);
                max_fd = clt_socks[i] > max_fd ? clt_socks[i] : max_fd;
            }
        }

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int ret = select(max_fd+1, &rfds, NULL, NULL, &tv);
        if(ret == 0){
            printf("there was no activity for 5 seconds\n");
            continue;
        }
        else if(ret == -1){
            perror("select error:");
            break;
        }
        else{
            if(ret > 0 && FD_ISSET(server_sock, &rfds)){
                memset(&client,0, sizeof(struct sockaddr_in));
                clt_socks[s] = accept(server_sock, (struct sockaddr *) &client,&len);

                if(clt_socks[s] == -1){
                    perror("accept failed: ");
                    continue;
                }
                s++;
            }


        }
        for(int i = 0; i< CLIENTS_COUNT; i++){
            if(FD_ISSET(clt_socks[i], &rfds)){
                err = read(clt_socks[i],&value,sizeof(value));
                if(err == -1){
                    perror("read failed: ");
                    break;
                }

                err = write(clt_socks[i],&value,sizeof(value));
                if(err == -1){
                    perror("write failed: ");
                    break;
                }
            }
        }
        printf("%d\n", value);
        sleep(1);
    }

    for (int i = 0; i < CLIENTS_COUNT; ++i) {
		close(clt_socks[i]);
	}

    close(server_sock);
    printf("Hello, World!");
    return 0;
}
