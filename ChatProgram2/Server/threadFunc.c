#include "../includeSet.h"
#include "chatInfo.h"

#define TRUE 1
#define FALSE 0

void *room_thread(void *arg);

//lobby function
void *lobby_thread(void *arg)
{
    pthread_detach(pthread_self());

    struct InfoInterface *info = (struct InfoInterface *)arg;

    fd_set readfds;
    int ret, i, j;
    int bigger_fds = 0, bigger_index = 0;

    struct timeval timeout;

    char roomlist[256] = "<ChatRoom info>\n";
    char roominfo[256];

    char buf[BUFSIZ];

    char joinCmd[BUFSIZ];

    pthread_t room_thr;

    while (TRUE)
    {

        FD_ZERO(&readfds);
        for (i = 0; i < LOBBYMAX_USER; i++)
        {
            if ((*info).Lobby.user[i] >= 0 && (*info).Lobby.user[i] <= 10)
            {
                FD_SET((*info).Lobby.user[i], &readfds);

                if (bigger_fds < (*info).Lobby.user[i])
                {
                    bigger_fds = (*info).Lobby.user[i];
                    bigger_index = i;
                }
            }
        }

        //select timeout 3sec non-blocking
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        ret = select((*info).Lobby.user[bigger_index] + 1, &readfds, NULL, NULL, &timeout);

        switch (ret)
        {
        case -1:
            perror("lobby select error");
        case 0:
            //printf("lobby select return : 0\n");
            break;
        default:
            i = 0;
            while (ret > 0)
            {
                if (FD_ISSET((*info).Lobby.user[i], &readfds))
                {
                    memset(buf, 0, sizeof(buf));
                    recv((*info).Lobby.user[i], buf, sizeof(buf), 0);
                    printf("[MAIN] User %d send Message : %s\n", (*info).Lobby.user[i], buf);

                    // show chatroom list
                    if (strcmp(buf, "1") == 0)
                    {
                        for (int j = 0; j < ROOMMAX_CNT; j++)
                        {
                            if ((*info).Room[j].roomId >= 0 && (*info).Room[j].roomId <= ROOMMAX_CNT)
                            {
                                sprintf(roominfo, "[ID %d] %s (%d/%d)\n", (*info).Room[j].roomId, (*info).Room[j].roomName, (*info).Room[j].userCnt, ROOMMAX_USER);
                                strcat(roomlist, roominfo);
                            }
                        }

                        send((*info).Lobby.user[i], roomlist, strlen(roomlist) + 1, 0);
                        sprintf(roomlist, "< ChatRoom info >\n");
                    }
                    //create room
                    else if (strcmp(buf, "3") == 0)
                    {
                        recv((*info).Lobby.user[i], buf, sizeof(buf), 0);

                        if ((*info).Lobby.roomCnt > 4)
                        {
                            char *msg = "Sorry room is full\n";
                            send((*info).Lobby.user[i], msg, strlen(msg) + 1, 0);
                        }
                        else
                        {
                            strcpy((*info).Room[(*info).Lobby.roomCnt].roomName, buf);
                            pthread_create(&room_thr, NULL, room_thread, (void *)&(*info));
                            printf("[MAIN] Open room : [Ch.%d] %s\n", (*info).Lobby.roomCnt, buf);
                        }
                    }
                    //disconnect from server
                    else if (strcmp(buf, "4") == 0)
                    {
                        printf("[MAIN] User %d disconnet\n", (*info).Lobby.user[i]);
                        //close((*info).Lobby.user[i]);
                        (*info).Lobby.user[i] = EMPTY;
                        (*info).Lobby.userCnt--;

                        //Lobby.user sort
                        // int temp = 0;
                        // for (int k = 0; k < LOBBYMAX_USER; k++)
                        // {
                        //     if ((*info).Lobby.user[k] == EMPTY)
                        //     {
                        //         for (int j = k; j < LOBBYMAX_USER; j++)
                        //         {
                        //             if ((*info).Lobby.user[j] != EMPTY)
                        //             {
                        //                 (*info).Lobby.user[j] = temp;
                        //                 (*info).Lobby.user[i] = (*info).Lobby.user[j];
                        //                 temp = (*info).Lobby.user[i];
                        //             }
                        //         }
                        //     }
                        // }
                        //sort
                    }
                    //show menu
                    else if (strcmp(buf, "0") == 0)
                    {
                        send((*info).Lobby.user[i], MENU, strlen(MENU) + 1, 0);
                    }
                    //join room '2 <roomid>'
                    else
                    {
                        int roomid, roomUserCnt;
                        for (j = 0; j < ROOMMAX_CNT; j++)
                        {
                            roomid = (*info).Room[j].roomId;
                            roomUserCnt = (*info).Room[j].userCnt;
                            sprintf(joinCmd, "2 %d", roomid);
                            if (strcmp(buf, joinCmd) == 0)
                            {
                                (*info).Room[j].user[roomUserCnt] = (*info).Lobby.user[i];
                                (*info).Lobby.user[i] = EMPTY;
                                printf("[MAIN] User %d join Ch.%d\n", (*info).Room[j].user[roomUserCnt], roomid);
                                printf("[Ch.%d] New User : %d\n", roomid, (*info).Room[j].user[roomUserCnt]);
                                (*info).Room[j].userCnt++;
                                break;
                            }
                        }
                    }
                    ret--;
                }
                i++;
            }
            break;
        }
    }
}

//chatroom funtion
void *room_thread(void *arg)
{

    pthread_detach(pthread_self());

    struct InfoInterface *info = (struct InfoInterface *)arg;

    fd_set readfds;
    int ret, i, j;
    int bigger_fds = 0, bigger_index = 0;

    struct timeval timeout;

    char buf[BUFSIZ];
    char buf2[BUFSIZ];

    //set roomID
    (*info).Room[(*info).Lobby.roomCnt].roomId = (*info).Lobby.roomCnt;
    int roomid = (*info).Room[(*info).Lobby.roomCnt].roomId;
    (*info).Lobby.roomCnt++;

    while (TRUE)
    {
        FD_ZERO(&readfds);

        for (i = 0; i < ROOMMAX_USER; i++)
        {
            if ((*info).Room[roomid].user[i] >= 0 && (*info).Room[roomid].user[i] <= 10)
            {
                FD_SET((*info).Room[roomid].user[i], &readfds);

                if (bigger_fds < (*info).Room[roomid].user[i])
                {
                    bigger_fds = (*info).Room[roomid].user[i];
                    bigger_index = i;
                }
            }
        }

        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        ret = select((*info).Room[roomid].user[bigger_index] + 1, &readfds, NULL, NULL, &timeout);

        switch (ret)
        {
        case -1:
            perror("room select error");
        case 0:
            //printf("Ch.%d select return :0\n", roomid);
            break;
        default:
            i = 0;
            while (ret > 0)
            {
                if (FD_ISSET((*info).Room[roomid].user[i], &readfds))
                {
                    memset(buf, 0, sizeof(buf));
                    recv((*info).Room[roomid].user[i], buf, sizeof(buf), 0);
                    printf("[Ch.%d] User %d message:%s\n", roomid, (*info).Room[roomid].user[i], buf);

                    //QUIT from chatroom and go lobby
                    if (strcmp(buf, "quit") == 0)
                    {
                        printf("[Ch.%d] User %d remove from room\n", roomid, (*info).Room[roomid].user[i]);
                        //close((*info).Room[roomid].user[i]);
                        for (j = 0; j < LOBBYMAX_USER; j++)
                        {
                            if ((*info).Lobby.user[j] == EMPTY)
                            {
                                (*info).Lobby.user[j] = (*info).Room[roomid].user[i];
                                (*info).Lobby.userCnt++;
                                break;
                            }
                        }

                        for (j = 0; j < ROOMMAX_USER; j++)
                        {
                            if ((*info).Room[roomid].user[j] >= 0 && (*info).Room[roomid].user[j] <= 10)
                            {
                                if ((*info).Room[roomid].user[j] != (*info).Room[roomid].user[i])
                                {
                                    sprintf(buf2, "[%d] : %s", (*info).Room[roomid].user[i], buf);
                                    send((*info).Room[roomid].user[j], buf2, strlen(buf2) + 1, 0);
                                }
                            }
                        }

                        (*info).Room[roomid].user[i] = EMPTY;
                        (*info).Room[roomid].userCnt--;
                    }
                    //just one user in chatroom
                    else if ((*info).Room[roomid].userCnt <= 1)
                    {
                        printf("[Ch.%d] User %d is anlone..\n", roomid, (*info).Room[roomid].user[i]);
                    }
                    //Broadcast to chatroom member
                    else
                    {
                        printf("[Ch.%d] Broadcast user %d message\n", roomid, (*info).Room[roomid].user[i]);
                        for (j = 0; j < ROOMMAX_USER; j++)
                        {
                            if ((*info).Room[roomid].user[j] >= 0 && (*info).Room[roomid].user[j] <= 10)
                            {
                                if ((*info).Room[roomid].user[j] != (*info).Room[roomid].user[i])
                                {
                                    sprintf(buf2, "[%d] : %s", (*info).Room[roomid].user[i], buf);
                                    send((*info).Room[roomid].user[j], buf2, strlen(buf2) + 1, 0);
                                }
                            }
                        }
                    }
                    ret--;
                }
                i++;
            }
        }
    }
}

//initialize function
void initialize(struct InfoInterface Info)
{
    int i, j;
    for (i = 0; i < ROOMMAX_CNT; i++)
    {
        Info.Room[i].roomId = 0;
        Info.Room[i].userCnt = 0;
        for (j = 0; j < ROOMMAX_USER; j++)
        {
            Info.Room[i].user[j] = EMPTY;
        }
    }

    // for (i = 0; i < USERMAX_CNT; i++)
    // {
    //     Info.User[i].userId = 0;
    //     Info.User[i].userState = 0;
    // }

    Info.Lobby.userCnt = 0;
    Info.Lobby.roomCnt = 0;
    for (i = 0; i < LOBBYMAX_USER; i++)
    {
        Info.Lobby.user[i] = EMPTY;
    }
}