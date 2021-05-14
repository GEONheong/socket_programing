#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#define N_CLIENT 3

int main(int argc, char *argv[])
{
    int access_sock, accept_sock[N_CLIENT];
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buf[256];

    fd_set readfds;
    int ret, i;

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

    for (i = 0; i < N_CLIENT; i++)
    {
        if ((accept_sock[i] = accept(access_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("socket accept error");
            exit(1);
        }
        else
        {
            printf("Client #%d connected.\n", i + 1);
        }

        int flag = fcntl(accept_sock[i], F_GETFL, 0);
        fcntl(accept_sock[i], F_SETFL, flag | O_NONBLOCK);
    }

    while (1)
    {
        FD_ZERO(&readfds);
        for (i = 0; i < N_CLIENT; i++)
        {
            FD_SET(accept_sock[i], &readfds);
        }
        printf("waiting at select...\n");
        ret = select(accept_sock[2] + 1, &readfds, NULL, NULL, NULL);
        printf("select returned : %d\n", ret);

        switch (ret)
        {
        case -1:
            perror("select error");
        case 0:
            printf("select returns :0\n");
            break;
        default:
            i = 0;
            while (ret > 0)
            {
                if (FD_ISSET(accept_sock[i], &readfds))
                {
                    memset(buf, 0, sizeof(buf));
                    recv(accept_sock[i], buf, sizeof(buf), 0);
                    ret--;
                    printf("MSG from Client %d: %s\n\n", i, buf);
                }
                i++;
            }
            break;
        }
    }

    for (i = 0; i < N_CLIENT; i++)
    {
        close(accept_sock[i]);
    }

    return 0;
}