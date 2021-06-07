#include "./includeSet.h"
#include "./commomResource.h"

void make_cards(struct card *cardset)
{
    int i = 0;
    printf("Make CardSet\n");

    for (i = 0; i < 14; i++)
    {
        if (i >= 0 && i < 5)
        {
            cardset[i].value = 1;
            cardset[i + 14].value = 1;
            cardset[i + 28].value = 1;
            cardset[i + 42].value = 1;
        }
        else if (i >= 5 && i < 8)
        {
            cardset[i].value = 2;
            cardset[i + 14].value = 2;
            cardset[i + 28].value = 2;
            cardset[i + 42].value = 2;
        }
        else if (i >= 8 && i < 11)
        {
            cardset[i].value = 3;
            cardset[i + 14].value = 3;
            cardset[i + 28].value = 3;
            cardset[i + 42].value = 3;
        }
        else if (i >= 11 && i < 13)
        {
            cardset[i].value = 4;
            cardset[i + 14].value = 4;
            cardset[i + 28].value = 4;
            cardset[i + 42].value = 4;
        }
        else if (i >= 13 && i < 14)
        {
            cardset[i].value = 5;
            cardset[i + 14].value = 5;
            cardset[i + 28].value = 5;
            cardset[i + 42].value = 5;
        }

        cardset[i].suit = 'B';
        cardset[i + 14].suit = 'L';
        cardset[i + 28].suit = 'S';
        cardset[i + 42].suit = 'P';
    }

    for (i = 0; i < 56; i++)
    {
        printf("(%c,%d)", cardset[i].suit, cardset[i].value);
        if (i == 13 || i == 27 || i == 41)
        {
            printf("\n");
        }
    }

    printf("\n\n");
}

void shuffle(struct card *cardset, int num)
{
    srand(time(NULL));

    struct card temp;

    int randInt;
    int i = 0;
    printf("Shuffing the cards\n");

    for (i = 0; i < num - 1; i++)
    {
        randInt = rand() % (num - i) + i;
        temp = cardset[i];
        cardset[i] = cardset[randInt];
        cardset[randInt] = temp;
    }
    for (i = 0; i < 56; i++)
    {
        printf("(%c,%d)", cardset[i].suit, cardset[i].value);
        if (i == 13 || i == 27 || i == 41)
        {
            printf("\n");
        }
    }

    printf("\n\n");
}