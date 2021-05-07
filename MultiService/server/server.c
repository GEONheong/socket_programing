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

extern char *get_localtime();
extern int sendfile(int sock, char *msg);
extern int echo_server(int sock);

int main(int argc, char *argv[])
{

    int access_sock, accept_sock;
    struct sockaddr_in server_addr, client_addr;
    char buf[256];
    char buf2[256];

    int client_addr_len = sizeof(client_addr);

    char *serverlist = "[Server List]\n1. Get Current Time\n2. Download File\n3. Echo Server\nEnter: ";
    char *filelist = "[Available File List]\n1. Book.txt\n2. HallymUniv.jpg\n3. Go back\nEnter: ";

    if ((access_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("access failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(access_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind failed");
        exit(1);
    }

    if (listen(access_sock, 5) == -1)
    {
        perror("listen failed");
        exit(1);
    }

    if ((accept_sock = accept(access_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
    {
        perror("accept failed");
        exit(1);
    }
    else
    {
        printf("client connected...\n");
    }

    while (1)
    {
        sleep(1);
        send(accept_sock, serverlist, strlen(serverlist) + 1, 0);

        recv(accept_sock, buf, sizeof(buf), 0);
        printf("%s\n", buf);

        if ((strcmp(buf, "\\server 1")) == 0)
        {
            char *time = get_localtime();
            send(accept_sock, time, strlen(time) + 1, 0);
            continue;
        }
        else if ((strcmp(buf, "\\server 2")) == 0)
        {
            send(accept_sock, filelist, strlen(filelist) + 1, 0);

            recv(accept_sock, buf2, sizeof(buf2), 0);

            if ((strcmp(buf2, "\\file 1")) == 0)
            {
                sendfile(accept_sock, buf2);
                continue;
            }
            else if ((strcmp(buf2, "\\file 2")) == 0)
            {
                sendfile(accept_sock, buf2);
                continue;
            }
            else if ((strcmp(buf2, "\\file 3")) == 0)
            {
                continue;
            }
        }
        else if ((strcmp(buf, "\\server 3")) == 0)
        {
            echo_server(accept_sock);
            continue;
        }
        else if ((strcmp(buf, "\\ server exec")) == 0)
        {
            continue;
        }
    }

    close(accept_sock);
    return 0;
}