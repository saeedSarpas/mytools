/*
 * memory/allocate_test.c
 * test_for: memory/allocate.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "allocate.h"


Describe(allocate);
BeforeEach(allocate) {}
AfterEach(allocate) {}


Ensure(allocate, works_properly)
{
  double *ptr = allocate(sizeof(double), 2);
  assert_that(ptr, is_non_null);
  free(ptr);
}


TestSuite *allocate_tests()
{
  TestSuite *suite = create_test_suite();
  add_test_with_context(suite, allocate, works_properly);
  return suite;
}
