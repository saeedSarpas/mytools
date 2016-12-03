/*
 * vector/vector_push_test.c
 * test_for: vector/vector_push.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "vector_push.h"


#define NUM_OF_ELEMS 13


Describe(vector_push);
BeforeEach(vector_push) {}
AfterEach(vector_push) {}


Ensure(vector_push, pushes_elements_to_vector_properly)
{
  vector *double_v = new_vector(2, sizeof(double));

  double dummy = 1.23456789;

  int i;
  for(i = 0; i < 4; i++)
    vector_push(double_v, &dummy);

  assert_that(double_v->elems, is_non_null);
  assert_that(double_v->allocLength, is_equal_to(4));
  assert_that(double_v->logLength, is_equal_to(4));

  dispose_vector(&double_v);
}
