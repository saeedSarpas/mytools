/*
 * fittingfunctions/fityaebx_test.c
 * test_for: fittingfunctions/fityaebx.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "fityaebx.h"
#include "./../../memory/allocate.h"
#include "./../../vector/vector_get.h"
#include "./../../vector/vector_push.h"


#define NUM_OF_ELEMS 20
#define COEFF_A 0.123
#define COEFF_B 0.56


Describe(fityaebx);
BeforeEach(fityaebx) {}
AfterEach(fityaebx) {}


Ensure(fityaebx, returns_the_opened_file)
{
  vector *xs = new_vector(NUM_OF_ELEMS, sizeof(float));
  vector *ys = new_vector(NUM_OF_ELEMS, sizeof(float));

  int i;
  float x, y;
  for(i = 0; i < NUM_OF_ELEMS; i++){
    x = (float)(i + 1);
    y = COEFF_A * exp(COEFF_B * x);
    vector_push(xs, &x);
    vector_push(ys, &y);
  }

  float *coeffs = fityaebx(xs, ys);

  assert_that_double(coeffs[0], is_equal_to_double(COEFF_A));
  assert_that_double(coeffs[1], is_equal_to_double(COEFF_B));

  free(coeffs);
}
