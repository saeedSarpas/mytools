/*
 * simulation/chech_if_saved_exist_test.c
 * test_for: simulation/check_if_saved_exist.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <stdlib.h>
#include "check_if_saved_exist.h"
#include "./../io/open_file.h"


#define FILENAME "testfile.ascii"


Describe(check_if_saved_exist);


BeforeEach(check_if_saved_exist)
{
  char *saved_file = saved_snap_path(FILENAME);
  FILE *fp = open_file(saved_file, "w");
  fprintf(fp, "Hello...\n");
  fclose(fp);
  free(saved_file);
};


AfterEach(check_if_saved_exist)
{
  remove(saved_snap_path(FILENAME));
};


Ensure(check_if_saved_exist, works_properly)
{
  assert_true(check_if_saved_exist(FILENAME));
  assert_false(check_if_saved_exist("NON_EXISTANCE.FILE"));
}
