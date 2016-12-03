/*
 * vector/vector_sort_test.c
 * test_for: vector/vector_sort.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "vector_sort.h"
#include "vector_push.h"
#include "vector_get.h"


#define NUM_OF_ELEMS 7


int compare(const void*, const void*);


Describe(vector_sort);
BeforeEach(vector_sort) {}
AfterEach(vector_sort) {}


Ensure(vector_sort, sortes_elements_to_vector_properly)
{
  vector *int_v = new_vector(8, sizeof(int));

  int i;
  for(i = NUM_OF_ELEMS; i > 0; i--)
    vector_push(int_v, &i);

  vector_sort(int_v, compare);

  for(i = 1; i < NUM_OF_ELEMS; i++)
    assert_true(*(int*)vector_get(int_v, i) > *(int*)vector_get(int_v, i-1));

  dispose_vector(&int_v);
}


int compare(const void *v1, const void *v2)
{
  return *(int*)v1 - *(int*)v2;
}
