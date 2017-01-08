/*
 * massassignmentfunctions/cic/cic.c
 * test_file: massassignmentfunctions/cic/cic_test.c
 *
 * Cloud in cell mass assignment function
 *
 * param: S snapshot containing particles
 * param: *grid_dims number of grids in each axis
 * param: *box_lengths length of the simulation box
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include "./cic.h"
#include "./../../memory/allocate.h"
#include "./../../grid/row_major_order/three_to_one.h"


static double cic_weigth(int*, double, double);

double* cic(snapshot *S, int *grid_dims, double *box_lengths)
{
  int i, j, k, n, index;
  int di, dj, dk;
  double i_weight, j_weight, k_weight, overlap, grid_lengths[3];

  int tot_num_of_grids = grid_dims[0] * grid_dims[1] * grid_dims[2];

  for(i = 0; i < 3; i++)
    grid_lengths[i] = box_lengths[i] / grid_dims[i];

  double *grid_mass = allocate(tot_num_of_grids, sizeof(double));

  for (n = 0; n < S->header->tot_nparticles; n++) {
    i = S->particles[n].pos[0] / grid_lengths[0];
    overlap = fmodf(S->particles[n].pos[0], grid_lengths[0]);
    i_weight = cic_weigth(&di, overlap, grid_lengths[0]);

    j = S->particles[n].pos[1] / grid_lengths[1];
    overlap = fmodf(S->particles[n].pos[1], grid_lengths[1]);
    j_weight = cic_weigth(&dj, overlap, grid_lengths[1]);

    k = S->particles[n].pos[2] / grid_lengths[2];
    overlap = fmodf(S->particles[n].pos[2], grid_lengths[2]);
    k_weight = cic_weigth(&dk, overlap, grid_lengths[2]);

    index = three_to_one(i, j, k, grid_dims);
    grid_mass[index] += i_weight * j_weight * k_weight * S->particles[n].mass;

    index = three_to_one(i+di, j, k, grid_dims);
    grid_mass[index] += (1 - i_weight) * j_weight * k_weight * S->particles[n].mass;

    index = three_to_one(i, j+dj, k, grid_dims);
    grid_mass[index] += i_weight * (1 - j_weight) * k_weight * S->particles[n].mass;

    index = three_to_one(i, j, k+dk, grid_dims);
    grid_mass[index] += i_weight * j_weight * (1 - k_weight) * S->particles[n].mass;

    index = three_to_one(i+di, j+dj, k, grid_dims);
    grid_mass[index] += (1 - i_weight) * (1 - j_weight) * k_weight * S->particles[n].mass;

    index = three_to_one(i, j+dj, k+dk, grid_dims);
    grid_mass[index] += i_weight * (1 - j_weight) * (1 - k_weight) * S->particles[n].mass;

    index = three_to_one(i+di, j, k+dk, grid_dims);
    grid_mass[index] += (1 - i_weight) * j_weight * (1 - k_weight) * S->particles[n].mass;

    index = three_to_one(i+di, j+dj, k+dk, grid_dims);
    grid_mass[index] += (1 - i_weight) * (1 - j_weight) * (1 - k_weight) * S->particles[n].mass;
  }

  return grid_mass;
}


static double cic_weigth(int *d_index, double overlap, double grid_length)
{
  double weight, grid_half_size = grid_length / 2.0;

  if (overlap > grid_half_size) {
    weight = 1 - (overlap / grid_length) + 0.5;
    *d_index = +1;
  } else {
    weight = (overlap / grid_length) + 0.5;
    *d_index = -1;
  }

  return weight;
}
