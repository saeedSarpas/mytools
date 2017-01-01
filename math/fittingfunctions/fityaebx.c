/*
 * math/fittingfunctions/fityaebx.c
 * test_file: math/fittingfunctions/fityaebx_test.c
 *
 * Fitting an exponential function in the form of Y = a e^bX to a given vectors
 * of points
 *
 * Parameters
 * xs: (float) vector of x s
 * ys: (float) vector of y s
 *
 * Return
 * fitting coefficients, a and b
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include "fityaebx.h"
#include "./../../memory/allocate.h"
#include "./../../vector/vector_get.h"


float* fityaebx(vector *xs, vector *ys)
{
  float *coeffs = allocate(2, sizeof(float));

  int i;
  float logY, sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0;

  for(i = 0; i < xs->logLength; i++){
    logY = log(*(float*)vector_get(ys, i));

    sumX += *(float*)vector_get(xs, i);
    sumX2 += pow(*(float*)vector_get(xs, i), 2);

    sumY += logY;
    sumXY += *(float*)vector_get(xs, i) * logY;
  }

  float A=(sumX2 * sumY - sumX * sumXY) / (xs->logLength * sumX2 - sumX * sumX);
  coeffs[0] = exp(A);

  coeffs[1] = (xs->logLength * sumXY - sumX * sumY)
    / (xs->logLength * sumX2 - sumX * sumX);

  return coeffs;
}
