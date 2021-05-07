#include <stdio.h>
#include <time.h>

char *get_localtime()
{
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  return asctime(timeinfo);
}