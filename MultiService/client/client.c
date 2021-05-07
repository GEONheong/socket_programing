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

#define IP_ADDR "127.0.0.1"

#define OPTION1 "\\server 1"
#define OPTION2 "\\server 2"
#define OPTION3 "\\server 3"
#define EXCEPTION "\\server exce"

#define FILE1 "\\file 1"
#define FILE2 "\\file 2"
#define FILE3 "\\file 3"

extern int recvfile(int sock, int num);
extern int echo_client(int sock);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server_addr;
    char buf[256];

    int num;
    int num2;
    int c;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("error");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect failed");
        exit(1);
    }
    else
    {
        printf("server connected...\n");
    }

    while (1)
    {
        recv(sock, buf, sizeof(buf), 0);
        printf("%s", buf);

        scanf("%d", &num);

        if (num == 1)
        {
            send(sock, OPTION1, strlen(OPTION1) + 1, 0);

            recv(sock, buf, sizeof(buf), 0);
            printf("Current local time and date: %s", buf);
            continue;
        }
        else if (num == 2)
        {
            send(sock, OPTION2, strlen(OPTION2) + 1, 0);

            recv(sock, buf, sizeof(buf), 0);
            printf("%s", buf);

            //recv file
            scanf("%d", &num2);
            if (num2 == 1)
            {
                send(sock, FILE1, strlen(FILE1) + 1, 0);
                recvfile(sock, num2);
                continue;
            }
            else if (num2 == 2)
            {
                send(sock, FILE2, strlen(FILE2) + 1, 0);
                recvfile(sock, num2);
                continue;
            }
            else if (num2 == 3)
            {
                send(sock, FILE3, strlen(FILE3) + 1, 0);
                continue;
            }
        }
        else if (num == 3)
        {
            send(sock, OPTION3, strlen(OPTION3) + 1, 0);
            echo_client(sock);
            continue;
        }
        else
        {
            send(sock, EXCEPTION, strlen(EXCEPTION) + 1, 0);
            continue;
        }
    }

    close(sock);
    return 0;
}