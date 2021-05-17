#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <pthread.h>

int access_sock, accept_sock;
struct sockaddr_in server_addr, client_addr;
int client_addr_len = sizeof(client_addr);
int client_cnt = 0;
char buf[256];

pthread_t t_id;
void *clientHandler(void *arg);

int main(int argc, char *argv[])
{

    if ((access_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket open error");
        exit(1);
    }

    memset((char *)&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(access_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("socket bind error");
        exit(1);
    }

    if (listen(access_sock, 5) == -1)
    {
        perror("socket listen error");
        exit(1);
    }

    while (1)
    {
        if ((accept_sock = accept(access_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("socket accept error");
            exit(1);
        }
        else
        {
            printf("New Client\n");
            client_cnt++;
            printf("Number of service client : %d\n", client_cnt);
        }

        if (pthread_create(&t_id, NULL, clientHandler, (void *)&accept_sock) < 0)
        {
            perror("thread create error");
            exit(1);
        }
    }
    close(access_sock);
    return 0;
}

void *clientHandler(void *arg)
{
    pthread_detach(pthread_self());

    int clnt_sock = *((int *)arg);

    while (1)
    {
        recv(clnt_sock, buf, sizeof(buf), 0);
        printf("Recv from client : %s\n\n", buf);
        if (strcmp(buf, "\\quit") == 0)
        {
            client_cnt--;
            printf("Client_cnt : %d...\n\n", client_cnt);
            break;
        }
        send(clnt_sock, buf, strlen(buf) + 1, 0);
    }

    close(clnt_sock);
    return 0;
}