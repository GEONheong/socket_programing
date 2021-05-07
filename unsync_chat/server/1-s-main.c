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
#include <sys/un.h>

#define PATH "./sock_addr"

int main(int argc, char *argv[])
{
    int access_sock, accept_sock;
    struct sockaddr_un server_addr, client_addr;
    char buf[256];

    int client_addr_len = sizeof(client_addr);

    if ((access_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("error : ");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, PATH);

    if (bind(access_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind failed : ");
        exit(1);
    }

    if (listen(access_sock, 5) == -1)
    {
        perror("listen failed : ");
        exit(1);
    }
    else
    {
        printf("[Info] Unix socket : Wating for conn req\n");
    }

    if ((accept_sock = accept(access_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
    {
        perror("error : ");
        exit(1);
    }
    else
    {
        printf("[Info] Unix socket : client connected\n");
    }

    int inet_access_sock, inet_accept_sock;
    struct sockaddr_in inet_server_addr, inet_client_addr;
    char buf2[256];

    int inet_client_addr_len = sizeof(inet_client_addr);

    if ((inet_access_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("access failed");
        exit(1);
    }

    inet_server_addr.sin_family = AF_INET;
    inet_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(inet_access_sock, (struct sockaddr *)&inet_server_addr, sizeof(inet_server_addr)) == -1)
    {
        perror("bind failed");
        exit(1);
    }

    if (listen(inet_access_sock, 5) == -1)
    {
        perror("listen failed");
        exit(1);
    }
    else
    {
        printf("[Info] Inet socket : Wating for conn req\n");
    }

    if ((inet_accept_sock = accept(inet_access_sock, (struct sockaddr *)&inet_client_addr, &inet_client_addr_len)) == -1)
    {
        perror("accept failed");
        exit(1);
    }
    else
    {
        printf("[Info] Inet socket : client connected\n");
    }

    int flag = fcntl(accept_sock, F_GETFL, 0);
    fcntl(accept_sock, F_SETFL, flag | O_NONBLOCK);

    flag = fcntl(inet_accept_sock, F_GETFL, 0);
    fcntl(inet_accept_sock, F_SETFL, flag | O_NONBLOCK);

    while (1)
    {
        if (recv(accept_sock, buf, sizeof(buf), 0) > 0)
        {
            printf("[ME] : %s\n", buf);
            send(inet_accept_sock, buf, strlen(buf) + 1, 0);
        }
        else if (recv(inet_accept_sock, buf2, sizeof(buf2), 0) > 0)
        {
            printf("[YOU] : %s\n", buf2);
        }

        if ((strcmp(buf, "\\quit")) == 0)
        {
            printf("[Server] %s\n", buf);
            printf("[Info] Closing socket");
            break;
        }
    }

    close(accept_sock);
    close(inet_accept_sock);

    return 0;
}