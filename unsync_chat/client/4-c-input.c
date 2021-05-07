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

#define PATH "./sock_addr2"

int main()
{
    int sock;
    struct sockaddr_un server_addr;
    char buf[256];

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("error : ");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, PATH);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect failed : ");
        exit(1);
    }
    else
    {
        printf("[Info] Inet socket : connected to the server\n");
    }

    int flag = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flag | O_NONBLOCK);

    while (1)
    {

        printf("Enter :");
        gets(buf);
        send(sock, buf, strlen(buf) + 1, 0);

        if ((strcmp(buf, "\\quit")) == 0)
        {
            if (close(sock) == -1)
            {
                perror("close error");
                exit(1);
            }
            else
            {
                printf("Terminate...\n: Success\n");
                printf("[Info] Closing socket");
                system("rm sock_addr2");
            }
            break;
        }
    }

    return 0;
}