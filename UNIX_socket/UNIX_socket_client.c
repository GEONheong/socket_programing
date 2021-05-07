#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/un.h>



int main()
{
    int sock;
    struct sockaddr_un server_addr;
    char buf[256];

    if((sock = socket(AF_UNIX,SOCK_STREAM,0)) == -1 ){
        perror("error : ");
        exit(1);
    }

    server_addr.sun_family =AF_UNIX;
    strcpy(server_addr.sun_path,"./sock_addr");

    if(connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1){
        perror("connect failed : ");
        exit(1);
    }
    else{
        printf("server connected...\n");
    }

    while(1){

        printf("sendMsg :");
        gets(buf);
        send(sock,buf,strlen(buf)+1,0);
        
        if((strcmp(buf,"\\quit"))==0){
            break;
        }
    }

    close(sock);
    
    return 0;
}