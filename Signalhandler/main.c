#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler()
{
    printf("Handler is called!\n");
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGINT, handler);
    sleep(1000);
    printf("Wake Up!\n");
    return 0;
}
