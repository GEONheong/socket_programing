#include "./includeSet.h"
#include "./commomResource.h"

#define SOCKPATH "./UNIX_OPEN_SOCK"

//func val
extern void make_cards(struct card *cardset);
extern void shuffle(struct card *cardset, int num);
extern void goout();

//socket val
int access_sock, accept_sock;
struct sockaddr_un server_addr, client_addr;
char buf[256];
int client_addr_len = sizeof(client_addr);

//commanResource val
struct gameinfo info;
struct card temp;

char sbuf[BUFSIZ] = "< Card list >\n";
char tbuf[BUFSIZ];

char cardlist_0[BUFSIZ] = "";
char cardlist_1[BUFSIZ] = "";

int num = 0;

//select() val
fd_set readfds;
int ret;

int j = 0;

int main()
{
    // game setting
    make_cards(info.cardSet);
    shuffle(info.cardSet, 56);
    info.userCnt = 0;
    info.topindex = 0;

    //socket
    if ((access_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket open error");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKPATH);

    if (bind(access_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("socket bind fail");
        exit(1);
    }

    if (listen(access_sock, 5) == -1)
    {
        perror("socket listen error");
        exit(1);
    }

    //ctrl + c signal
    signal(SIGINT, goout);

    //accept 2 user
    for (int i = 0; i < MAXUSER; i++)
    {
        if ((accept_sock = accept(access_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
        {
            perror("socket accept error");
            exit(1);
        }
        else
        {
            info.users[info.userCnt].userSocknum = accept_sock;
            printf("[INFO] User %d Join Game!\n", info.userCnt);

            //fisrt onhandcard
            int userid = info.userCnt;

            info.users[userid].onHandcnt = 0;

            for (int i = 0; i < (MAXCARD / MAXUSER); i++)
            {
                info.users[userid].onHand[i] = info.cardSet[info.topindex];
                info.users[userid].onHandcnt++;
                sprintf(tbuf, "%d : (%c, %d), ", i, info.users[userid].onHand[i].suit, info.users[userid].onHand[i].value);
                strcat(sbuf, tbuf);
                info.topindex++;
            }

            send(info.users[userid].userSocknum, sbuf, strlen(sbuf) + 1, 0);
            strcpy(sbuf, "< Card list >\n");

            info.userCnt++;
        }
    }
    /////////////////////////////////////////////////////////////////
    ///////////////////////////GAME START////////////////////////////
    /////////////////////////////////////////////////////////////////
    while (true)
    {
        /////////////////////////////////////////////////////////////
        ///////////////////////USER0 TURN////////////////////////////
        /////////////////////////////////////////////////////////////
    USER0:
        //start turn 0user
        sleep(1);
        send(info.users[0].userSocknum, TURNSTART, strlen(TURNSTART) + 1, 0);

        //send cardlist
        sleep(1);
        send(info.users[0].userSocknum, cardlist_1, strlen(cardlist_1) + 1, 0);

        //recv cardindex & dropcard index
        recv(info.users[0].userSocknum, buf, sizeof(buf), 0);
        num = atoi(buf);

        temp = info.users[0].onHand[num];

        for (int i = num; i < info.users[0].onHandcnt; i++)
        {
            info.users[0].onHand[i].value = info.users[0].onHand[i + 1].value;
            info.users[0].onHand[i].suit = info.users[0].onHand[i + 1].suit;
        }

        info.users[0].onHandcnt--;
        info.opencard[0] = temp;

        printf("[USER 0]Card (%c,%d) is dropped\n", temp.suit, temp.value);

        //print buf open card
        strcpy(cardlist_0, "              ( OPEN CARD )\n");
        sprintf(tbuf, "USER %d : (%c , %d) <---->", info.users[0].userSocknum - 4, info.opencard[0].suit, info.opencard[0].value);
        strcat(cardlist_0, tbuf);
        sprintf(tbuf, " (%c , %d) : USER %d \n", info.opencard[1].suit, info.opencard[1].value, info.users[1].userSocknum - 4);
        strcat(cardlist_0, tbuf);
        sprintf(tbuf, "\nLeft Card : %d \n", info.users[0].onHandcnt);
        strcat(cardlist_0, tbuf);

        //game end if onhandcard == 0
        if (info.users[0].onHandcnt == 0)
        {
            if (info.users[0].score > info.users[1].score)
            {
                send(info.users[0].userSocknum, WIN, strlen(WIN) + 1, 0);
                send(info.users[1].userSocknum, LOSE, strlen(LOSE) + 1, 0);
            }
            else if (info.users[0].score < info.users[1].score)
            {
                send(info.users[1].userSocknum, WIN, strlen(WIN) + 1, 0);
                send(info.users[0].userSocknum, LOSE, strlen(LOSE) + 1, 0);
            }
        }
        else // match open card , press bell event
        {
            if (info.opencard[0].suit == info.opencard[1].suit) //shape is same
            {
                if ((info.opencard[0].value + info.opencard[1].value) == 5) //total value == 5 --> event
                {
                    send(info.users[0].userSocknum, PRESS, strlen(PRESS) + 1, 0);
                    send(info.users[1].userSocknum, PRESS, strlen(PRESS) + 1, 0);

                    // AFTER PRESS
                    FD_ZERO(&readfds);

                    FD_SET(info.users[0].userSocknum, &readfds);
                    FD_SET(info.users[1].userSocknum, &readfds);

                    ret = select(info.users[1].userSocknum + 1, &readfds, NULL, NULL, NULL);

                    switch (ret)
                    {
                    case -1:
                        perror("select error\n");
                    default:
                        j = 0;
                        while (ret > 0)
                        {
                            if (FD_ISSET(info.users[j].userSocknum, &readfds))
                            {
                                recv(info.users[j].userSocknum, buf, sizeof(buf), 0);
                                printf("USER %d PRESS!!\n", info.users[j].userSocknum - 4);

                                if (strcmp(buf, "p") == 0)
                                {
                                    info.users[j].score += 5;
                                    printf("USER %d SCORE : %d\n", info.users[j].userSocknum - 4, info.users[j].score);

                                    send(info.users[0].userSocknum, RING, strlen(RING) + 1, 0);
                                    send(info.users[1].userSocknum, RING, strlen(RING) + 1, 0);
                                    goto USER1;
                                }
                                ret--;
                            }
                            j++;
                        }
                    }
                }
            }
            else
            {
                if (info.opencard[0].value == 5 || info.opencard[1].value == 5) //one card value 5
                {
                    send(info.users[0].userSocknum, PRESS, strlen(PRESS) + 1, 0);
                    send(info.users[1].userSocknum, PRESS, strlen(PRESS) + 1, 0);

                    // PRESS AFTER
                    FD_ZERO(&readfds);

                    FD_SET(info.users[0].userSocknum, &readfds);
                    FD_SET(info.users[1].userSocknum, &readfds);

                    ret = select(info.users[1].userSocknum + 1, &readfds, NULL, NULL, NULL);

                    switch (ret)
                    {
                    case -1:
                        perror("select error\n");
                    default:
                        j = 0;
                        while (ret > 0)
                        {
                            if (FD_ISSET(info.users[j].userSocknum, &readfds))
                            {
                                recv(info.users[j].userSocknum, buf, sizeof(buf), 0);
                                printf("USER %d PRESS!!\n", info.users[j].userSocknum - 4);

                                if (strcmp(buf, "p") == 0)
                                {
                                    info.users[j].score += 5;
                                    printf("USER %d SCORE : %d\n", info.users[j].userSocknum - 4, info.users[j].score);
                                    send(info.users[0].userSocknum, RING, strlen(RING) + 1, 0);
                                    send(info.users[1].userSocknum, RING, strlen(RING) + 1, 0);
                                    goto USER1;
                                }
                                ret--;
                            }
                            j++;
                        }
                    }
                }
            }
        }

        /////////////////////////////////////////////////////////////
        ///////////////////////USER1 TURN////////////////////////////
        /////////////////////////////////////////////////////////////
    USER1:
        //turn user1
        sleep(1);
        send(info.users[1].userSocknum, TURNSTART, strlen(TURNSTART) + 1, 0);

        //send cardlist
        sleep(1);
        send(info.users[1].userSocknum, cardlist_0, strlen(cardlist_0) + 1, 0);

        //recv cardindex & dropcard index
        recv(info.users[1].userSocknum, buf, sizeof(buf), 0);
        num = atoi(buf);

        //update onhandcard
        temp = info.users[1].onHand[num];

        for (int i = num; i < info.users[1].onHandcnt; i++)
        {
            info.users[1].onHand[i].value = info.users[1].onHand[i + 1].value;
            info.users[1].onHand[i].suit = info.users[1].onHand[i + 1].suit;
        }

        info.users[1].onHandcnt--;
        info.opencard[1] = temp;

        printf("[USER 1]Card (%c,%d) is dropped\n", temp.suit, temp.value);

        //print buf, open card
        strcpy(cardlist_1, "              ( OPEN CARD )\n");
        sprintf(tbuf, "USER %d : (%c , %d) <---->", info.users[0].userSocknum - 4, info.opencard[0].suit, info.opencard[0].value);
        strcat(cardlist_1, tbuf);
        sprintf(tbuf, " (%c , %d) : USER %d \n", info.opencard[1].suit, info.opencard[1].value, info.users[1].userSocknum - 4);
        strcat(cardlist_1, tbuf);
        sprintf(tbuf, "\nLeft Card : %d \n", info.users[1].onHandcnt);
        strcat(cardlist_1, tbuf);

        //game end if onhandcard is 0
        if (info.users[1].onHandcnt == 0)
        {
            if (info.users[0].score > info.users[1].score)
            {
                send(info.users[0].userSocknum, WIN, strlen(WIN) + 1, 0);
                send(info.users[1].userSocknum, LOSE, strlen(LOSE) + 1, 0);
            }
            else if (info.users[0].score < info.users[1].score)
            {
                send(info.users[1].userSocknum, WIN, strlen(WIN) + 1, 0);
                send(info.users[0].userSocknum, LOSE, strlen(LOSE) + 1, 0);
            }
        }
        else // match open card , press bell event
        {
            if (info.opencard[0].suit == info.opencard[1].suit) //shape is same
            {
                if ((info.opencard[0].value + info.opencard[1].value) == 5) //total value == 5 --> event
                {
                    send(info.users[0].userSocknum, PRESS, strlen(PRESS) + 1, 0);
                    send(info.users[1].userSocknum, PRESS, strlen(PRESS) + 1, 0);

                    // PRESS AFTER
                    FD_ZERO(&readfds);

                    FD_SET(info.users[0].userSocknum, &readfds);
                    FD_SET(info.users[1].userSocknum, &readfds);

                    ret = select(info.users[1].userSocknum + 1, &readfds, NULL, NULL, NULL);

                    switch (ret)
                    {
                    case -1:
                        perror("select error\n");
                    default:
                        j = 0;
                        while (ret > 0)
                        {
                            if (FD_ISSET(info.users[j].userSocknum, &readfds))
                            {
                                recv(info.users[j].userSocknum, buf, sizeof(buf), 0);
                                printf("USER %d PRESS!!\n", info.users[j].userSocknum - 4);

                                if (strcmp(buf, "p") == 0)
                                {
                                    info.users[j].score += 5;
                                    printf("USER %d SCORE : %d\n", info.users[j].userSocknum - 4, info.users[j].score);
                                    send(info.users[0].userSocknum, RING, strlen(RING) + 1, 0);
                                    send(info.users[1].userSocknum, RING, strlen(RING) + 1, 0);
                                    //WIN score logic
                                    goto USER0;
                                }
                                ret--;
                            }
                            j++;
                        }
                    }

                    //
                }
            }
            else
            {
                if (info.opencard[0].value == 5 || info.opencard[1].value == 5) //one card value 5
                {
                    send(info.users[0].userSocknum, PRESS, strlen(PRESS) + 1, 0);
                    send(info.users[1].userSocknum, PRESS, strlen(PRESS) + 1, 0);

                    // PRESS AFTER
                    FD_ZERO(&readfds);

                    FD_SET(info.users[0].userSocknum, &readfds);
                    FD_SET(info.users[1].userSocknum, &readfds);

                    ret = select(info.users[1].userSocknum + 1, &readfds, NULL, NULL, NULL);

                    switch (ret)
                    {
                    case -1:
                        perror("select error\n");
                    default:
                        j = 0;
                        while (ret > 0)
                        {
                            if (FD_ISSET(info.users[j].userSocknum, &readfds))
                            {
                                recv(info.users[j].userSocknum, buf, sizeof(buf), 0);
                                printf("USER %d PRESS!!\n", info.users[j].userSocknum - 4);

                                if (strcmp(buf, "p") == 0)
                                {
                                    info.users[j].score += 5;
                                    printf("USER %d SCORE : %d\n", info.users[j].userSocknum - 4, info.users[j].score);
                                    send(info.users[0].userSocknum, RING, strlen(RING) + 1, 0);
                                    send(info.users[1].userSocknum, RING, strlen(RING) + 1, 0);
                                    //WIN score logic
                                    goto USER0;
                                }
                                ret--;
                            }
                            j++;
                        }
                    }

                    //
                }
            }
        }
    }

    return 0;
}