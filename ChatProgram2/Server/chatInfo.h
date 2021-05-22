#include "../includeSet.h"

#define TRUE 1
#define FALSE 0

#define MENU "<MENU>\n1.Chatting Room List\n2.Join Catting Room(Usage: 2 <Chatting Room Number>)\n3.Create Room\n4.Disconnect from server\n(0.Try again)\n"

#define ROOMMAX_USER 5
#define LOBBYMAX_USER 10

#define ROOMMAX_CNT 5
#define USERMAX_CNT 10

#define EMPTY 999

struct Room
{
    int roomId;
    char roomName[256];
    int user[ROOMMAX_USER];
    int userCnt;
};

struct Lobby
{
    int userCnt;
    int user[LOBBYMAX_USER];
    int roomCnt
};

// struct User
// {
//     int userId;
//     int userState;
// };

struct InfoInterface
{
    struct Room Room[ROOMMAX_CNT];
    struct Lobby Lobby;
    //struct User User[USERMAX_CNT];
};
