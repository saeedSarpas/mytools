/*
 * memory/copy_test.c
 * test_for: memory/copy.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "copy.h"


Describe(copy);
BeforeEach(copy) {}
AfterEach(copy) {}


#define VALUE 1.2345


Ensure(copy, works_properly)
{
  double *from = malloc(1 * sizeof(double));
  double *to = malloc(1 * sizeof(double));

  *from = VALUE;

  copy(from, to, sizeof(double));
  assert_that(*to, is_equal_to(VALUE));

  free(from);
  free(to);
}
