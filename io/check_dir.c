/*
 * io/check_dir.c
 * test_file:
 *
 * Checking whether a directory exist or not
 *
 * param: path path to the directory
 *
 * return: a boolean (CD_DIR_EXIST or CD_DIR_NONEXIST)
 *
 * author: Saeed Sarpas
 */


#include <string.h>
#include <dirent.h>
#include "./check_dir.h"


int check_dir(const char *path)
{
  char *copied_path = strdup(path);
  DIR* dir = opendir(copied_path);

  if(dir)
    return CD_DIR_EXIST;
  else
    return CD_DIR_NONEXIST;
}
