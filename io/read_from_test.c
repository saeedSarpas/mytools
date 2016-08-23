/*
 * io/read_from_test.c
 * test_for: io/read_from.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "read_from.h"


Describe(read_from);


#define BIN_FILE_ADDR "./test.bin"
float PI = 3.141592;


BeforeEach(read_from)
{
  FILE *file;
  file = fopen(BIN_FILE_ADDR, "wb");

  fwrite(&PI, 1, sizeof(float), file);

  fclose(file);
}


AfterEach(read_from)
{
  remove(BIN_FILE_ADDR);
}


Ensure(read_from, reads_correctly)
{
  FILE *fp = fopen(BIN_FILE_ADDR, "rb");
  float returned_value;
  read_from(fp, 1, sizeof(float), &returned_value);
  assert_that(returned_value, is_equal_to(PI));
  fclose(fp);
}


TestSuite *read_from_tests()
{
  TestSuite *suite = create_test_suite();
  add_test_with_context(suite, read_from, reads_correctly);
  return suite;
}
