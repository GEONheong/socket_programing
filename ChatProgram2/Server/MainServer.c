#include "../includeSet.h"
#include "chatInfo.h"

//function
extern void handler();
extern void *lobby_thread(void *arg);
extern void initialize(struct InfoInterface Info);

//struct var
struct InfoInterface Info;

//state var
int curUserCnt = 0;

//socket var
int access_sock;
struct sockaddr_in server_addr, client_addr;
int clinet_addr_len = sizeof(client_addr);
char buf[BUFSIZ];

//nonblock var
int flag;

//thread var
pthread_t lobby_thr;

int main(int argc, char *argv[])
{

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
        perror("bind error");
        exit(1);
    }

    if (listen(access_sock, 15) == -1)
    {
        perror("listen error");
        exit(1);
    }

    signal(SIGINT, handler);

    //intialize all var
    initialize(Info);

    //lobby thread
    pthread_create(&lobby_thr, NULL, lobby_thread, (void *)&Info);

    while (TRUE)
    {
        //accept new user
        if ((Info.Lobby.user[Info.Lobby.userCnt] = accept(access_sock, (struct sockaddr *)&client_addr, &clinet_addr_len)) == -1)
        {
            perror("accept error");
            exit(1);
        }
        else
        {
            send(Info.Lobby.user[Info.Lobby.userCnt], MENU, strlen(MENU) + 1, 0);

            printf("[MAIN] New User connect :%d\n", Info.Lobby.user[Info.Lobby.userCnt]);

            Info.Lobby.userCnt++;
        }
    }
}