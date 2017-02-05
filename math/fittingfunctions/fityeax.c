/*
 * math/fittingfunctions/fityeax.c
 * test_file: math/fittingfunctions/fityeax_test.c
 *
 * Fitting a linear function without intercept
 *
 * Parameters
 * xs: (float) vector of x s
 * ys: (float) vector of y s
 *
 * Return
 * fitting coefficients, a
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include "fityeax.h"
#include "./../../memory/allocate.h"
#include "./../../vector/vector_get.h"


fitresultyeax* fityeax(vector *xs, vector *ys)
{
  fitresultyeax *result = allocate(1, sizeof(*result));

  unsigned int i;
  double x, y, xy = 0, x2 = 0;

  for(i = 0; i < xs->logLength; i++){
    x = *(float*)vector_get(xs, i);
    y = *(float*)vector_get(ys, i);

    xy += x * y;
    x2 += x * x;
  }

  result->slope = xy / x2;

  return result;
}
