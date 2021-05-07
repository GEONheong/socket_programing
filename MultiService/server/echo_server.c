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

int echo_server(int sock)
{
    char buf[256];

    while (1)
    {
        recv(sock, buf, sizeof(buf), 0);
        printf("[client]: %s\n", buf);
        if ((strcmp(buf, "\\quit")) == 0)
        {
            break;
        }
        send(sock, buf, strlen(buf) + 1, 0);
    }

    return 0;
}