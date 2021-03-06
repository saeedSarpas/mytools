/*
 * io/open_file_test.c
 * test_for: io/open_file.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "open_file.h"


Describe(open_file);


#define BIN_FILE_ADDR "./test.bin"


BeforeEach(open_file)
{
  FILE *file;
  file = fopen(BIN_FILE_ADDR, "wb");

  char x[10]="ABCDEFGHIJ";
  fwrite(x, sizeof(x[0]), sizeof(x)/sizeof(x[0]), file);

  fclose(file);
}


AfterEach(open_file)
{
  remove(BIN_FILE_ADDR);
}


Ensure(open_file, returns_the_opened_file)
{
  FILE *fp = open_file(BIN_FILE_ADDR, "rb");
  assert_that(fp, is_non_null);
  fclose(fp);
}
