/*
 * io/write_to_test.c
 * test_for: io/write_to.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "write_to.h"
#include "read_from.h"


Describe(write_to);


#define BIN_FILE_ADDR "./write_to_test.bin"
#define VALUE_TO_WRITE 1.23456789


BeforeEach(write_to) {}
AfterEach(write_to) {}


Ensure(write_to, writes_data_to_file_correctly_correctly)
{
  FILE *file = fopen(BIN_FILE_ADDR, "w");
  double value = VALUE_TO_WRITE;
  write_to(file, 1, sizeof(double), &value);
  fclose(file);

  double r_value;
  FILE *fp = fopen(BIN_FILE_ADDR, "r");
  fread(&r_value, sizeof(double), 1, fp);
  printf("%f\n", r_value);
  assert_that_double(r_value, is_equal_to_double(VALUE_TO_WRITE));
  fclose(fp);

  remove(BIN_FILE_ADDR);
}
