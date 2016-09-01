/*
 * grid/row_major_order/three_to_one_test.c
 * test_for: grid/row_major_order/three_to_one.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "./three_to_one.h"


Describe(three_to_one);
BeforeEach(three_to_one){}
AfterEach(three_to_one){}


#define NUM_OF_GRIDS 64
#define XLEN 10
#define YLEN 11
#define ZLEN 12


Ensure(three_to_one, returns_right_results_for_a_trivial_input)
{
  int i, j, k = ZLEN / 2, index, dims[3] = {XLEN, YLEN, ZLEN};

  for(i = 0; i < 2 * XLEN; i++)
    for(j = 0; j < YLEN; j++){
      index = three_to_one(i, j, k, dims);
      assert_that(index, is_equal_to((i % XLEN) * (YLEN * ZLEN) + j * ZLEN + k));
    }
}
