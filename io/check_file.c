/*
 * io/check_file.c
 * test_file:
 *
 * Checking whether a file exist or not
 *
 * param: path path to the file
 *
 * return: a boolean (CF_FILE_EXIST or CF_FILE_NONEXIST)
 *
 * author: Saeed Sarpas
 */


#include <string.h>
#include <unistd.h>
#include "./check_file.h"


int check_file(const char *path)
{
  char *copied_path = strdup(path);
  if(access(copied_path, F_OK) != -1)
    return CF_FILE_EXIST;
  else
    return CF_FILE_NONEXIST;
}
