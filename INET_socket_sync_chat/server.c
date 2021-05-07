#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc ,char* argv[])
{
    int access_sock, accept_sock;
    struct sockaddr_in server_addr, client_addr;
    char buf[256];

    int client_addr_len = sizeof(client_addr);

    if((access_sock = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("access failed");
        exit(1);
    }


    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if(bind(access_sock,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1){
        perror("bind failed");
        exit(1);
    }

    if(listen(access_sock,5)==-1){
        perror("listen failed");
        exit(1);
    }

    if((accept_sock=accept(access_sock,(struct sockaddr*)&client_addr,&client_addr_len)) == -1){
        perror("accept failed");
        exit(1);
    }
    else{
        printf("client connected...\n");
    }


    while(1){
        recv(accept_sock,buf,sizeof(buf),0);
        printf("[YOU] %s\n",buf);

        if((strcmp(buf,"\\quit"))==0){
            break;
        }

        printf("[ME] : ");
        gets(buf);
        send(accept_sock,buf,strlen(buf)+1,0);

        if((strcmp(buf,"\\quit"))==0){
            break;
        }
    }

    close(accept_sock);

    return 0;
}