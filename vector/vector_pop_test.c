/*
 * vector/vector_pop_test.c
 * test_for: vector/vector_pop.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "vector_push.h"
#include "vector_pop.h"


#define NUM_OF_ELEMS 7


Describe(vector_pop);
BeforeEach(vector_pop) {}
AfterEach(vector_pop) {}


Ensure(vector_pop, popes_elements_to_vector_properly)
{
  vector *int_v = new_vector(8, sizeof(int));

  int i;
  for(i = 0; i < NUM_OF_ELEMS; i++)
    vector_push(int_v, &i);

  for(i = NUM_OF_ELEMS; i > 0; i--){
    int *elem = vector_pop(int_v);
    assert_that(*elem, is_equal_to(i - 1));
  }

  dispose_vector(&int_v);
}
