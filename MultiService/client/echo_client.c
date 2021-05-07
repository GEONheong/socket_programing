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

int echo_client(int sock)
{
    char buf[256];
    while (1)
    {
        printf("send: ");
        //scanf("%s", buf);
        //memset(buf, '\0', 256);
        getchar();
        char *ret = fgets(buf, 256, stdin);

        printf("1. %s (%d)\n", buf, strlen(buf));
        printf("2. %s (%d)\n", ret, strlen(ret));

        send(sock, buf, strlen(buf) + 1, 0);
        if ((strcmp(buf, "\\quit")) == 0)
        {
            break;
        }
        recv(sock, buf, sizeof(buf), 0);
        printf("[YOU]: %s\n", buf);
    }
    return 0;
}