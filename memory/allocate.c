/*
 * memory/allocate.c
 * test_file: memoru/allocate_test.c
 *
 * Allocating memory
 *
 * param: nmemb number of elements to be allocated
 * param: size size of elements
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "allocate.h"


void* allocate(unsigned int nmemb, unsigned int size)
{
  void *pptr = calloc(nmemb, size);
  if (pptr == NULL)
    printf("[calloc error: %s]\n", strerror(errno));

  return pptr;
}
