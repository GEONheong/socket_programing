#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int access_sock, accept_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    int client_cnt = 0;
    char buf[256];

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

        if (fork() == 0)
        {
            close(access_sock);
            while (1)
            {
                recv(accept_sock, buf, sizeof(buf), 0);
                printf("Recv from client : %s\n\n", buf);
                if (strcmp(buf, "\\quit") == 0)
                {
                    printf("Client-%d disconneted...\n\n", client_cnt);
                    break;
                }
                send(accept_sock, buf, strlen(buf) + 1, 0);
            }
            exit(0);
        }

        close(accept_sock);
    }

    return 0;
}