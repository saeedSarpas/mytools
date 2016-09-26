/*
 * grid/row_major_roder/point_to_grid.c
 * test_file: grid/row_major_roder/point_to_grid_test.c
 *
 * calculating the grid index of a point-like object (e.g. a particle)
 *
 * param: x, y, z coordinate of the point
 * param: box array containing the dimensions of the box
 * param: dims array containing the dimensions of the grid
 *
 * author: saeed sarpas
 */


#include "point_to_grid.h"
#include "three_to_one.h"


int point_to_grid(double x, double y, double z, double *box, int *grid)
{
  int i, j, k;

  i = x * grid[0] / box[0];
  j = y * grid[1] / box[1];
  k = z * grid[2] / box[2];

  return three_to_one(i, j, k, grid);
}
