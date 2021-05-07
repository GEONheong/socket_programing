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
    int access_sock, accept_sock;
    struct sockaddr_un server_addr, client_addr;
    char buf[256];

    int client_addr_len = sizeof(client_addr);

    if((access_sock = socket(AF_UNIX,SOCK_STREAM,0)) == -1 ){
        perror("error : ");
        exit(1);
    }

    server_addr.sun_family =AF_UNIX;
    strcpy(server_addr.sun_path,"./sock_addr");

    if(bind(access_sock,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1){
        perror("bind failed : ");
        exit(1);
    }

    if(listen(access_sock,5) == -1){
        perror("listen failed : ");
        exit(1);
    }

    if((accept_sock = accept(access_sock,(struct sockaddr*)&client_addr,&client_addr_len)) == -1){
        perror("error : ");
        exit(1);
    }
    else{
        printf("Client connected...\n");
    }

    while(1){
        recv(accept_sock,buf,sizeof(buf),0);
        printf("recvMsg :%s\n",buf);

        if((strcmp(buf,"\\quit"))==0){
            break;
        }
        
    }

    close(accept_sock);
    
    return 0;
}