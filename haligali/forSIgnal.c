#include "./includeSet.h"

void goout()
{
    printf("\nCtrl + C is commanded...\n");
    system("rm UNIX_OPEN_SOCK");
    exit(EXIT_SUCCESS);
}