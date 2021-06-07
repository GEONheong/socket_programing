#include "./includeSet.h"

#define true 1
#define false 0

#define MAXUSER 2
#define MAXCARD 56

#define TURNSTART "turns"
#define TURNEND "turne"
#define PRESS "press"
#define WIN "win"
#define LOSE "lose"

#define RING "ring"

struct card
{
    int value;
    char suit;
};

struct user
{
    int score;
    int userSocknum;
    int onHandcnt;
    struct card onHand[MAXCARD];
};

struct gameinfo
{
    char checkbuf[BUFSIZ];
    int userCnt;
    int topindex;
    struct card cardSet[MAXCARD];
    struct card opencard[MAXUSER];
    struct user users[MAXUSER];
};
