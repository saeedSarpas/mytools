/*
 * io/write_to.c
 * test_file: io/write_to_test.c
 *
 * Writing to file
 *
 * param: file
 * param: nmemb number of elements
 * param: size size of elements
 * param: ptr porinter to a block of memory with minimum size of nmemb * size
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "write_to.h"


void write_to(FILE *file, unsigned int nmemb, unsigned int size, void *ptr)
{
  if(fwrite(ptr, size, nmemb, file) != nmemb)
    printf("[fwrite error: %s]\n", strerror(errno));
}
