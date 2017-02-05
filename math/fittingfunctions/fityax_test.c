/*
 * fittingfunctions/fityax_test.c
 * test_for: fittingfunctions/fityax.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "fityax.h"
#include "./../../vector/vector_push.h"


#define NUM_OF_ELEMS 17
#define COEFF_A 0.123


Describe(fityax);
BeforeEach(fityax) {}
AfterEach(fityax) {}


Ensure(fityax, returns_the_opened_file)
{
  vector *xs = new_vector(NUM_OF_ELEMS, sizeof(float));
  vector *ys = new_vector(NUM_OF_ELEMS, sizeof(float));

  int i;
  float x, y;
  for(i = 0; i < NUM_OF_ELEMS; i++){
    x = (float)i;
    y = COEFF_A * x;
    vector_push(xs, &x);
    vector_push(ys, &y);
  }

  fitresultyax *result = fityax(xs, ys);

  assert_that_double(result->slope, is_equal_to_double(COEFF_A));

  free(result);
}
