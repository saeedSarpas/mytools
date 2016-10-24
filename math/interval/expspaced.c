/*
 * math/interval/expspaced.c
 * test_file: math/interval/expspaced_test.c
 *
 * Generating exponential bin edges between two given numbers
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include "expspaced.h"
#include "./../../memory/allocate.h"


double* expspaced(double min, double max, int n)
{
  int i;

  double *edges = allocate(n, sizeof(double));

  edges[0] = min;
  edges[n] = max;

  double root = pow(n + 1, 1./(max - min));

  for(i = 1; i < n; i++)
    edges[i] = min + log10(i + 1) / log10(root);

  return edges;
}
