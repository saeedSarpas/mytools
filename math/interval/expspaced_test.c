/*
 * math/interval/expspaced_test.c
 * test_for: math/interval/expspaced.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "expspaced.h"


Describe(expspaced);
BeforeEach(expspaced) {}
AfterEach(expspaced) {}


#define LEN 16


Ensure(expspaced, generate_right_numbers_for_a_trivial_input)
{
  int i;
  double *edges = expspaced(0, 49105, LEN);

  for(i = 0; i < LEN - 1; i++)
    assert_true(edges[i+1] - edges[i] > edges[i+2] - edges[i+1]);

  free(edges);
}
