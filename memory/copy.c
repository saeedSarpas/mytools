/*
 * memory/copy.c
 * test_file: memoru/copy_test.c
 *
 * Allocating memory
 *
 * param: from pointer to the place to be read
 * param: to pointer to the polace to be written
 * param: size size of the block to be copied
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "copy.h"


void copy(void *from, void *to, unsigned int size)
{
  if(memcpy(to, from, size) == NULL)
    printf("[memcpy error: %s]\n", strerror(errno));
}
