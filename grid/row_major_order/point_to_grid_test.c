/*
 * grid/row_major_order/point_to_grid_test.c
 * test_for: grid/row_major_order/point_to_grid.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "point_to_grid.h"
#include "three_to_one.h"


Describe(point_to_grid);
BeforeEach(point_to_grid){}
AfterEach(point_to_grid){}


#define XLEN 120.0
#define YLEN 110.0
#define ZLEN 100.0
#define GRIDX 8
#define GRIDY 9
#define GRIDZ 10


Ensure(point_to_grid, returns_right_results_for_a_trivial_input)
{
  int i, j, k, grid_index;
  double box_lengths[3] = {XLEN, YLEN, ZLEN};
  int grid_dims[3] = {GRIDX, GRIDY, GRIDZ};
  double x, y, z;

  for(i = 0; i < GRIDX; i += 3)
    for(j = 0; j < GRIDY; j += 3)
      for(k = 0; k < GRIDZ; k++){
        x = i * (box_lengths[0] / grid_dims[0] * (1 + GRIDX / 100.0));
        y = j * (box_lengths[1] / grid_dims[1] * (1 + GRIDY / 100.0));
        z = k * (box_lengths[2] / grid_dims[2] * (1 + GRIDZ / 100.0));
        grid_index = point_to_grid(x, y, z, box_lengths, grid_dims);
        assert_that(grid_index, is_equal_to(three_to_one(i, j, k, grid_dims)));
      }
}
