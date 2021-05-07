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

#define IP_ADDR "127.0.0.1"

int main(int argc, char* argv[]){
    int sock;
    struct sockaddr_in server_addr;
    char buf[256];

    if((sock=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("error");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
    server_addr.sin_port = htons(atoi(argv[1]));

    if(connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1){
        perror("connect failed");
        exit(1);
    }
    else{
        printf("server connected...\n");
    }

    while(1){
        printf("[ME] : ");
        gets(buf);
        send(sock,buf,strlen(buf)+1,0);

        if((strcmp(buf,"\\quit"))==0){
            break;
        }

        recv(sock,buf,sizeof(buf),0);
        printf("[YOU] : %s\n",buf);

        if((strcmp(buf,"\\quit"))==0){
            break;
        }
    }

    close(sock);

    return 0;
}