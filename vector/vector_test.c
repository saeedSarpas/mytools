/*
 * vector/vector_test.c
 * test_for: vector/vector.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "vector.h"


#define NUM_OF_ELEMS 13


Describe(vector);
BeforeEach(vector) {}
AfterEach(vector) {}


Ensure(vector, initializes_a_new_vector_properly)
{
  vector *double_v = new_vector(NUM_OF_ELEMS, sizeof(double));

  assert_that(double_v, is_non_null);
  assert_that(double_v->elems, is_non_null);
  assert_that(double_v->elemSize, is_equal_to(sizeof(double)));
  assert_that(double_v->allocLength, is_equal_to(NUM_OF_ELEMS));
  assert_that(double_v->logLength, is_equal_to(0));

  dispose_vector(&double_v);
}


Ensure(vector, disposes_properly)
{
  vector *double_v = new_vector(NUM_OF_ELEMS, sizeof(double));
  double_v->logLength = 3;

  dispose_vector(&double_v);

  assert_that(double_v, is_null);
}
