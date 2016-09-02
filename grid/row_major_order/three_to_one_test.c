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


static int dims[3] = {XLEN, YLEN, ZLEN};


Ensure(three_to_one, returns_right_results_for_a_trivial_inputs)
{
  int i, j, k = ZLEN / 2, index;

  for(i = 0; i < 2 * XLEN; i++)
    for(j = 0; j < YLEN; j++){
      index = three_to_one(i, j, k, dims);
      assert_that(index, is_equal_to((i % XLEN) * (YLEN * ZLEN) + j * ZLEN + k));
    }
}


Ensure(three_to_one, returns_right_result_for_negative_inputs)
{
  int index;
  index = three_to_one(-1, 0, 0, dims);
  assert_that(index, is_equal_to((XLEN - 1) * YLEN * ZLEN));

  index = three_to_one(-1 * XLEN - 1, 0, 0, dims);
  assert_that(index, is_equal_to((XLEN - 1) * YLEN * ZLEN));

  index = three_to_one(0, -1, 0, dims);
  assert_that(index, is_equal_to((YLEN - 1) * ZLEN));

  index = three_to_one(0, -1 * YLEN - 1, 0, dims);
  assert_that(index, is_equal_to((YLEN - 1) * ZLEN));

  index = three_to_one(0, 0, -1, dims);
  assert_that(index, is_equal_to((ZLEN - 1)));

  index = three_to_one(0, 0, -1 * ZLEN - 1, dims);
  assert_that(index, is_equal_to((ZLEN - 1)));
}
