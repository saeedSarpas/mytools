/*
 * vector/vector_get_test.c
 * test_for: vector/vector_get.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "vector_push.h"
#include "vector_get.h"


#define NUM_OF_ELEMS 7


Describe(vector_get);
BeforeEach(vector_get) {}
AfterEach(vector_get) {}


Ensure(vector_get, getes_elements_to_vector_properly)
{
  vector *int_v = new_vector(8, sizeof(int));

  int i;
  for(i = 0; i < NUM_OF_ELEMS; i++)
    vector_push(int_v, &i);

  for(i = 0; i < NUM_OF_ELEMS; i++){
    int *elem = vector_get(int_v, i);
    assert_that(*elem, is_equal_to(i));
  }

  dispose_vector(&int_v);
}
