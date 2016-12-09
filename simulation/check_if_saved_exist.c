/*
 * simulation/gadget/check_if_saved_exist.c
 * test_file:
 *
 * Check if the saved version of an snapshot exist
 *
 * Parameters:
 * path: Path to the snapshot file
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "./check_if_saved_exist.h"
#include "./../strings/concat/concat.h"
#include "./../io/check_file.h"


bool check_if_saved_exist(char *path)
{
  char *saved_snap = saved_snap_path(path);

  if(check_file(saved_snap) == CF_FILE_EXIST){
    free(saved_snap);
    return true;
  }
  else{
    free(saved_snap);
    return false;
  }
}


char* saved_snap_path(char *path_to_snap)
{
  return concat(2, path_to_snap, SAVED_SUFFIX);
}
