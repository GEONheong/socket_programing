#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define MAXUSER 5

struct ChatInfo
{
    struct ChatRoom room;
    struct UserSockets new_users;
    struct UserSockets return_users;
};

struct ChatRoom
{
    int room_num;           //channel number
    int user_sock[MAXUSER]; //sock number array
    int user_cnt;           //now user count
};

struct UserSockets
{
    short is_new_user;    //boolean newUser?
    int new_user_sock;    //newUser sock
    short is_return_user; //boolean returnUser?
    int retunr_user_sock; //returnUser sock
};