/*
 * grid/row_major_order/one_to_three_test.c
 * test_for: grid/row_major_order/one_to_three.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "./one_to_three.h"


Describe(one_to_three);
BeforeEach(one_to_three){}
AfterEach(one_to_three){}


#define NUM_OF_GRIDS 64
#define XLEN 10
#define YLEN 11
#define ZLEN 12


Ensure(one_to_three, returns_right_results_for_a_trivial_input)
{
  int i, j, k = ZLEN / 2, *pos, dims[3] = {XLEN, YLEN, ZLEN};

  for(i = 0; i < 2 * XLEN; i++)
    for(j = 0; j < YLEN; j++){
      pos = one_to_three(i * (YLEN * ZLEN) + j * ZLEN + k, dims);
      assert_that(pos[0], is_equal_to(i % XLEN));
      assert_that(pos[1], is_equal_to(j % YLEN));
      assert_that(pos[2], is_equal_to(k % ZLEN));
    }
}
