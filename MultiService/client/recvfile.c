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
#include <errno.h>

int recvfile(int sock, int num)
{
    FILE *file = NULL;
    char buf[BUFSIZ];

    size_t filesize, recvsize;
    size_t totalrecvsize = 0;

    if (num == 1)
    {
        if ((file = fopen("Book.txt", "wb")) == NULL)
        {
            perror("file open error");
        }
    }
    else if (num == 2)
    {
        if ((file = fopen("HallymUniv.jpg", "wb")) == NULL)
        {
            perror("file open error");
        }
    }

    recvsize = sizeof(buf);

    while (recvsize != 0)
    {
        memset(buf, '\0', sizeof(buf));
        recvsize = recv(sock, buf, sizeof(buf), 0);
        //totalrecvsize += recvsize;
        //printf("recvsize:%ld\n", totalrecvsize);
        fwrite(buf, sizeof(char), recvsize, file);
        if (recvsize < sizeof(buf))
        {
            break;
        }
    }

    fclose(file);

    return 0;
}