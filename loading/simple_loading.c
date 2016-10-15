/*
 * loading/simple_loading.c
 * test_file: loading/simple_loading_test.c
 *
 * Simple loading module for indicating the progress of the computation
 *
 * param: percent the previous printed percentage
 * param: i the number of the element which is calculating
 * param: len the number of the whole elements
 *
 * return: the percentages that is printed
 *
 * TODO it's too messy, make it clear!
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include "simple_loading.h"


#define BARLENGTH 77
static char BAR[] = "==========================================================="
  "=================>";


int simple_loading(int percent, int i, int len)
{
  if(percent != i / (len / 100)){
    percent = i / (len / 100);

    int i, pointer = BARLENGTH - (percent * 100 / BARLENGTH);
    char revised_bar[BARLENGTH];

    for(i = pointer; i < BARLENGTH; i++)
      revised_bar[i - pointer] = BAR[i];

    for(i = BARLENGTH - pointer; i < BARLENGTH; i++)
      revised_bar[i] = ' ';

    printf("\t[%3d%%%s]\r", percent, revised_bar);
    fflush(stdout);
  }

  return percent;
}
