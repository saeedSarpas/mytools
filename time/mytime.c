/*
 * time/mytime.c
 * test_file: time/mytime_test.c
 *
 * managing time of processes
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <string.h>
#include "mytime.h"


clock_t start(char *msg)
{
  printf(msg, strlen(msg));
  fflush(stdout);

  return clock();
}


void done(clock_t start)
{
  clock_t finish = clock();
  double time = (double)(finish - start) / CLOCKS_PER_SEC;

  printf(" [done in %f sec]\n", time);
  fflush(stdout);

  return;
}
