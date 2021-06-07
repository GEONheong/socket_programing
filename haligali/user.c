#include "./includeSet.h"
#include "./commomResource.h"

#define SOCKPATH "./UNIX_OPEN_SOCK"
#define true 1
#define false 0

int sock;
struct sockaddr_un server_addr;
char buf[BUFSIZ];
char cardlist[BUFSIZ];
char ringbuf[BUFSIZ];

int num;

pthread_t t_id;

//thread function
void *input_thread()
{
    pthread_detach(pthread_self());

    char buf[BUFSIZ];

    gets(buf);
    send(sock, buf, strlen(buf) + 1, 0);
}

int main()
{
    //socket
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket open error");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKPATH);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("socket connect error");
        exit(1);
    }
    else
    {
        printf("[INFO] JOIN GAME!\n");
    }

    //recv user card list
    recv(sock, cardlist, sizeof(cardlist), 0);
    printf("%s\n", cardlist);

    ////////////////////////////GAME START//////////////////////////////
    while (true)
    {
        //recv message
        recv(sock, buf, sizeof(buf), 0);

        if (strcmp(buf, PRESS) == 0) //check press bell event
        {
            printf("\n\n\n------------PRESS FAST PRESS FAST !!!!-------------\n");
            printf("----------------- (input 'p') ---------------------\n");
            pthread_create(&t_id, NULL, input_thread, NULL);
            recv(sock, ringbuf, sizeof(ringbuf), 0);
            printf("\n\n----------------- RING RING !!~~~~ ---------------\n");
            pthread_cancel(t_id);
        }
        else if (strcmp(buf, TURNSTART) == 0) //turn start
        {
            //printf("%s\n", buf);
            recv(sock, buf, sizeof(buf), 0);
            printf("\n\n\n\n\n<<<<<<<<<<<<<<<< MY Turn >>>>>>>>>>>>>>>>\n");
            printf("%s\n", buf);
            printf("Enter:");
            gets(buf);
            send(sock, buf, strlen(buf) + 1, 0);
            printf("\n<<<<<<<<<<<<<<<< Turn End >>>>>>>>>>>>>>>\n");
        }
        else if (strcmp(buf, WIN) == 0) //win event
        {
            printf("############################\n");
            printf("########YOU WIN!!###########\n");
            printf("############################\n");
            system("rm UNIX_OPEN_SOCK");
            break;
        }
        else if (strcmp(buf, LOSE) == 0) //lose event
        {

            printf("############################\n");
            printf("########YOU LOSE...#########\n");
            printf("############################\n");
            break;
        }
    }

    close(sock);
    return 0;
}
