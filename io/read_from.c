/*
 * io/read_from.c
 * test_file: io/read_from_test.c
 *
 * Reading from a file
 *
 * param: file
 * param: nmemb number of elements
 * param: size size of elements
 * param: ptr porinter to a block of memory with minimum size of nmemb * size
 *
 * author: Saeed Sarpas
 */


#include "read_from.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>


void read_from(FILE *file, unsigned int nmemb, unsigned int size, void *ptr)
{
  if (fread(ptr, size, nmemb, file) != nmemb)
    printf("[fread error: %s]\n", strerror(errno));
}
