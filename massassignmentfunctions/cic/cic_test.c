/*
 * massassignmentfunctions/cic/cic_test.c
 * test_for: massassignmentfunctions/cic/cic.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "./cic.h"
#include "./../../memory/allocate.h"
#include "./../../grid/row_major_order/three_to_one.h"


Describe(cic);


#define NUM_OF_PARTS 10
#define PART_MASS 1
#define XDIM 10
#define XLENGTH 10
#define YDIM 11
#define YLENGTH 11
#define ZDIM 12
#define ZLENGTH 12


static double *grid_mass;
static snapshot *S;
static void fill_P(double, double, double, double);
static int grid_dims[3] = {XDIM, YDIM, ZDIM};
static double box_lengths[3] = {XLENGTH, YLENGTH, ZLENGTH};
static int tot_num_of_grids;


BeforeEach(cic)
{
  tot_num_of_grids = grid_dims[0] * grid_dims[1] * grid_dims[2];
  S = new_snapshot(NUM_OF_PARTS);
}


AfterEach(cic)
{
  dispose_snapshot(&S);
  free(grid_mass);
}


Ensure(cic, returns_right_values_for_a_trivial_case)
{
  fill_P(4.5, 4.5, 4.5, 1);

  grid_mass = cic(S, grid_dims, box_lengths);

  int i, j, k, index, filled_grid_index = three_to_one(4, 4, 4, grid_dims);

  for (i = -2; i <= 2; i++) {
    for (j = -2; j <= 2; j++) {
      for (k = -2; k <= 2; k++) {
        index = three_to_one(4 + i, 4 + j, 4 + k, grid_dims);
        if (index != filled_grid_index)
          assert_that(grid_mass[index], is_equal_to(0));
        else
          assert_that(grid_mass[index], is_equal_to(1 * NUM_OF_PARTS));
      }
    }
  }
}


Ensure(cic, calculates_the_share_of_each_cell)
{
  fill_P(4.0, 4.0, 4.0, 1);
  grid_mass = cic(S, grid_dims, box_lengths);

  int index, i, j, k;
  for (i = -2; i <= 1; i++) {
    for (j = -2; j <= 1; j++) {
      for (k = -2; k <= 1; k++) {
        index = three_to_one(4 + i, 4 + j, 4 + k, grid_dims);
        if (i == -2 || j == -2 || k == -2 || i == 1 || j == 1 || k == 1)
          assert_that(grid_mass[index], is_equal_to(0));
        else
          assert_that(grid_mass[index],
          is_equal_to(1 * NUM_OF_PARTS / 8));
      }
    }
  }
}


Ensure(cic, calculates_the_share_of_grids_at_borders)
{
  fill_P(4, 0, 4, 1);

  grid_mass = cic(S, grid_dims, box_lengths);

  int index = three_to_one(4, YDIM - 1, 4, grid_dims);
  assert_that(grid_mass[index],is_equal_to(1.25));
}


void fill_P(double x, double y, double z, double mass)
{
  int i;
  for (i = 0; i < NUM_OF_PARTS; i++) {
    S->particles[i].pos[0] = x;
    S->particles[i].pos[1] = y;
    S->particles[i].pos[2] = z;
    S->particles[i].mass = mass;
  }
}
