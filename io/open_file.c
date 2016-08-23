/*
 * io/open_file.c
 * test_file: io/open_file_test.c
 *
 * Opening file in different modes
 *
 * param: path relative path to the file
 * param: mode file access mode (e.g. r w a)
 * halos and rockstar particle ids.
 *
 * author: Saeed Sarpas
 */


#include "open_file.h"


FILE* open_file(char *path, char *mode)
{
  FILE *file;
  file = fopen(path, mode);

  if(file)
    return file;
  else {
    printf("[fopen error: %s (%s)]\n", strerror(errno), path);
    return NULL;
  }
}
