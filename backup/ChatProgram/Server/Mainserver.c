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
#include <signal.h>
#include <pthread.h>
#include "ChattingInfo.h"

#define TRUE 1
#define FALSE 0
#define ROOMMAX 3
#define LEAVINGROOMMAX 15

int access_sock, accept_sock;
struct sockaddr_in server_addr, client_addr;
int client_addr_len = sizeof(client_addr);
char buf[256];
int flag;

char *menu = "<MENU>\n1.Chatting Room List\n2.Join Catting Room(Usage: 2 <Chatting Room Number>)\n3.Disconnect from server\n(0.Try again)\n";
char roomlist[256] = "<ChatRoom info>\n";
char roominfo[256];
int leavingroom_sock[LEAVINGROOMMAX] = {NULL};
int maxsock_index;
pthread_t room_id[ROOMMAX];

void handler();
void *thread_func(void *arg);

struct ChatInfo chatinfo[3];

fd_set leavingfds;
int selectleaving;
struct timeval timeout;

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

    //create room(create thread)
    for (int i = 0; i < ROOMMAX; i++)
    {
        chatinfo[i].room.room_num = room_id;
        chatinfo[i].room.user_cnt = 0;
        for (int j = 0; j < MAXUSER; j++)
        {
            chatinfo[i].room.user_sock[j] = NULL;
        }
        chatinfo[i].new_users.is_new_user = FALSE;
        chatinfo[i].new_users.new_user_sock = NULL;
        chatinfo[i].return_users.is_return_user = FALSE;
        chatinfo[i].return_users.retunr_user_sock = NULL;

        pthread_create(&room_id[i], NULL, threadfunc, (void *)&chatinfo[i]);

        // sprintf(roominfo, "[ID: %d] Cahtroom-%d (%d/%d)\n", room_id, room_id, chatinfo[i].room.user_cnt, MAXUSER);
        // strcat(roomlist, roominfo);
        // printf("");
        // *roomlist = "<ChatRoom info>\n";
    }

    //leaving room
    while (1)
    {
        if ((accept_sock = accept(access_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("accept error");
            exit(1);
        }
        else
        {
            printf("[MAIN] New User connect :%d", access_sock);
            send(accept_sock, menu, strlen(menu) + 1, 0);
            for (int i = 0; i < LEAVINGROOMMAX; i++)
            {
                if (leavingroom_sock[i] == NULL)
                {
                    leavingroom_sock[i] = accept_sock;
                    break;
                }
            }
        }

        flag = fcntl(accept_sock, F_GETFL, 0);
        fcntl(accept_sock, F_SETFL, flag | O_NONBLOCK);

        //check quit user from chatroom
        for (int i = 0; i < ROOMMAX; i++)
        {
            if (chatinfo[i].return_users.is_return_user)
            {
                for (int j = 0; j < LEAVINGROOMMAX; j++)
                {
                    if (leavingroom_sock[j] == NULL)
                    {
                        leavingroom_sock[j] = chatinfo[i].return_users.retunr_user_sock;
                        break;
                    }
                }
            }
        }

        //join chatroom
        FD_ZERO(&leavingfds);
        for (int i = 0; i < LEAVINGROOMMAX; i++)
        {
            FD_SET(leavingroom_sock[i], &leavingfds);

            if ((i < LEAVINGROOMMAX - 1) && (leavingroom_sock[i] < leavingroom_sock[i + 1]))
            {
                maxsock_index = i + 1;
            }
        }
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        selectleaving = select(leavingroom_sock[maxsock_index] + 1, &leavingfds, NULL, NULL, &timeout);

        switch (selectleaving)
        {
        case -1:
            perror("leaving select error");
        case 0:
            printf("leaving select return : 0\n");
            break;
        default:
            int k = 0;
            while (selectleaving > 0)
            {
                if (FD_ISSET(leavingroom_sock[k], &leavingfds))
                {
                    memset(buf, 0, sizeof(buf));
                    recv(leavingroom_sock[k], buf, sizeof(buf), 0);
                    printf("[MAIN] User %d Message: %s", leavingroom_sock[k], buf);

                    if (strcmp(buf, "1") == 0)
                    {
                        for (int i = 0; i < ROOMMAX; i++)
                        {
                            sprintf(roominfo, "[ID: %d] Cahtroom-%d (%d/%d)\n", room_id[i], room_id[i], chatinfo[i].room.user_cnt, MAXUSER);
                            strcat(roomlist, roominfo);
                        }
                        send(leavingroom_sock[k], roomlist, strlen(roomlist) + 1, 0);
                        *roomlist = "<ChatRoom info>\n";
                    }
                    else if (stcmp(buf, "2") == 0)
                    {
                    }
                    else if (srtcmp(buf, "3") == 0)
                    {
                        close(leavingroom_sock[k]);
                        leavingroom_sock[k] = NULL;
                        //leaving room sort
                        int temp;
                        for (int i = 0; i < LEAVINGROOMMAX; i++)
                        {
                            if (leavingroom_sock[i] == NULL)
                            {
                                for (int j = i; j < LEAVINGROOMMAX; j++)
                                {
                                    if (leavingroom_sock[j] != NULL)
                                    {
                                        leavingroom_sock[j] = temp;
                                        leavingroom_sock[i] = leavingroom_sock[j];
                                        temp = leavingroom_sock[i];
                                    }
                                }
                            }
                        }
                        //sort
                    }
                    else if (stcmp(buf, "0") == 0)
                    {
                        send(leavingroom_sock[k], menu, strlen(menu) + 1, 0);
                    }
                }
                k++;
            }
        }
    }
}

void handler()
{
    printf("(Signal Handler) Terminate server ...\n");
    exit(EXIT_SUCCESS);
}

void *threadfunc(void *arg)
{
    pthread_detach(ptread_self());

    struct ChatInfo *chatinfo = (struct ChatInfo *)arg;
    int user_cnt;
    fd_set roomfds;
    int selectRecv, i;

    while (TRUE)
    {
        user_cnt = (*chatinfo).room.user_cnt;

        if ((*chatinfo).new_users.is_new_user)
        {
            (*chatinfo).room.user_cnt++;
            user_cnt = (*chatinfo).room.user_cnt;

            //join room newUser
            (*chatinfo).room.user_sock[user_cnt] = (*chatinfo).new_users.new_user_sock;
            pritnf("[MAIN] USER %d join chatroom-%d",
                   (*chatinfo).room.user_sock[user_cnt], (*chatinfo).room.room_num);

            //Initialize about newUser
            (*chatinfo).new_users.is_new_user = FALSE;
            (*chatinfo).new_users.new_user_sock = NULL;
        }

        FD_ZERO(&roomfds);
        for (i = 0; i < user_cnt; i++)
        {
            FD_SET((*chatinfo).room.user_sock[i], &roomfds);
        }

        selectRecv = select((*chatinfo).room.user_sock[user_cnt] + 1, &roomfds, NULL, NULL, NULL);

        switch (selectRecv)
        {
        case -1:
            perror("chatroom select error");
        case 0:
            perror("chatroom select return : 0\n");
            break;
        default:
            i = 0;
            while (selectRecv > 0)
            {
                if (FD_ISSET((*chatinfo).room.user_sock[i], &roomfds))
                {
                    //recv and send message in chatroom
                    memset(buf, 0, sizefo(buf));
                    recv((*chatinfo).room.user_sock[i], buf, sizeof(buf), 0);
                    printf("[Ch.%d] USER %d send Message : %s\n", (*chatinfo).room.room_num, (*chatinfo).room.user_sock[i], buf);
                    for (int j = 0; j < user_cnt; j++)
                    {
                        if (j == i)
                        {
                            continue;
                        }
                        else
                        {
                            send((*chatinfo).room.user_sock[j], buf, strlen(buf) + 1, 0);
                        }
                    }
                    //User quit room
                    if (strcmp(buf, "\\quit") == 0)
                    {
                        (*chatinfo).return_users.is_return_user = TRUE;
                        (*chatinfo).return_users.retunr_user_sock = (*chatinfo).room.user_sock[i];
                        (*chatinfo).room.user_sock[i] = NULL;
                        for (int k = i; k < user_cnt; k++)
                        {
                            (*chatinfo).room.user_sock[k] = (*chatinfo).room.user_sock[k + 1];
                        }
                        (*chatinfo).room.user_cnt--;

                        printf("[Ch.%d] USER %d detach from chatroom.\n", (*chatinfo).room.room_num, (*chatinfo).room.user_sock[i]);
                    }

                    selectRecv--;
                }
                i++;
            }
        }
    }
}