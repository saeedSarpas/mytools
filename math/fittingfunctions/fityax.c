/*
 * math/fittingfunctions/fityax.c
 * test_file: math/fittingfunctions/fityax_test.c
 *
 * Fitting a linear function without intercept
 *
 * Parameters
 * xs: (double) vector of x s
 * ys: (double) vector of y s
 *
 * Return
 * fitting coefficients, a
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include "fityax.h"
#include "./../../memory/allocate.h"
#include "./../../vector/vector_get.h"


fitresultyax* fityax(vector *xs, vector *ys)
{
  fitresultyax *result = allocate(1, sizeof(*result));

  unsigned int i;
  double x, y, xy = 0, x2 = 0;

  for(i = 0; i < xs->logLength; i++){
    x = *(double*)vector_get(xs, i);
    y = *(double*)vector_get(ys, i);

    xy += x * y;
    x2 += x * x;
  }

  result->slope = xy / x2;

  return result;
}
