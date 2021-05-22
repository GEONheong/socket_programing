#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void handler()
{
    printf("(Signal Handler) Terminate server ...\n");
    exit(EXIT_SUCCESS);
}
