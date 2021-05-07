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

int sendfile(int sock, char *msg)
{
    FILE *file = NULL;
    char buf[BUFSIZ];

    size_t filesize, sendsize = 0;

    if (strcmp(msg, "\\file 1") == 0)
    {
        if ((file = fopen("Book.txt", "rb")) == NULL)
        {
            perror("file open error");
        }
    }
    else if (strcmp(msg, "\\file 2") == 0)
    {
        if ((file = fopen("HallymUniv.jpg", "rb")) == NULL)
        {
            perror("file open error");
        }
    }

    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    printf("filesize : %ld\n", filesize);
    fseek(file, 0, SEEK_SET);

    while (sendsize != filesize)
    {
        memset(buf, '\0', sizeof(buf));
        int file_poiter_size = fread(buf, sizeof(char), sizeof(buf), file);
        sendsize += file_poiter_size;
        send(sock, buf, file_poiter_size, 0);
        //printf("sendsize:%ld\n", sendsize);
    }

    fclose(file);

    return 0;
}