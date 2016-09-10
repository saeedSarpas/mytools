/*
 * vector/vector_test.c
 * test_for: vector/vector.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "vector.h"


Describe(vector);
BeforeEach(vector) {}
AfterEach(vector) {}


#define DUMMY 1.23456789


Ensure(vector, creates_a_new_vector_properly)
{
  struct _vector *double_v = vector_new(20, sizeof(double));

  assert_that(double_v, is_non_null);
  assert_that(double_v->elems, is_non_null);
  assert_that(double_v->alloc_length, is_equal_to(20));
  assert_that(double_v->log_length, is_equal_to(0));

  vector_dispose(double_v);
}


Ensure(vector, pushes_elements_to_vector_properly)
{
  struct _vector *double_v = vector_new(2, sizeof(double));

  double dummy = 1.23456789;

  int i;
  for(i = 0; i < 4; i++)
    vector_push(double_v, &dummy);

  assert_that(double_v->elems, is_non_null);
  assert_that(double_v->alloc_length, is_equal_to(4));
  assert_that(double_v->log_length, is_equal_to(4));

  for(i = 0; i < 4; i++)
    assert_that(*(double*)vector_get_elem(double_v, i), is_equal_to(dummy));

  vector_dispose(double_v);
}


Ensure(vector, pops_the_vector_properly)
{
  struct _vector *double_v = vector_new(8, sizeof(double));

  double returned_val, dummy;

  int i;
  for(i = 0; i < 8; i++){
    dummy = i * DUMMY;
    vector_push(double_v, &dummy);
  }

  for(i = 0; i < 8; i++){
    assert_that(double_v->log_length, is_equal_to(8 - i));
    returned_val = *(double*)vector_pop(double_v);
    assert_that(double_v->alloc_length, is_equal_to(8));
    assert_that(returned_val, is_equal_to((8 - (i + 1)) * DUMMY));
  }

  vector_dispose(double_v);
}
