/*
 * halofinder/fill_init_volume_test.c
 * test_for: halofinder/fill_init_volume.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./fill_init_volume.h"
#include "./../halofinder/halofinder.h"
#include "./../vector/vector.h"
#include "./../vector/vector_get.h"
#include "./../memory/allocate.h"
#include "./../grid/row_major_order/point_to_grid.h"


#define BOXLEN 100
#define NUMHALOS 1
#define NUMPARTS 10
#define PARTMASS 1.2e9
#define GRIDX 100
#define GRIDY 100
#define GRIDZ 100


int dims[3] = {GRIDX, GRIDY, GRIDZ};
snapshot *s;
halofinder *hf;


Describe(fill_init_volume);


BeforeEach(fill_init_volume)
{
  // Generating snapshot
  s = new_snapshot(NUMPARTS);

  int i;
  for(i = 0; i < 6; i++){
    s->header->npart[i] = 0;
    s->header->mass[i] = 0.0;
  }
  s->header->npart[1] = NUMPARTS;

  s->header->time = 1.0;
  s->header->redshift = 0.0;

  s->header->boxsize = BOXLEN;

  for(i = 0; i < s->header->tot_nparticles; i++)
    s->particles[i].id = i;

  for(i = 0; i < s->header->tot_nparticles; i++){
    s->particles[i].pos[0] = 0.0;
    s->particles[i].pos[1] = 0.0;
    s->particles[i].pos[1] = 0.0;
  }

  // Generating halofinder with only one halo
  hf = new_halofinder(1);

  for(i = 0; i < 3; i++)
    hf->header->box_size[i] = BOXLEN;

  hf->halos[0].id = 0;
  hf->halos[0].num_p = NUMPARTS;
  allocate_particle_ids(&hf->halos[0], hf->halos[0].num_p);

  // Filling halo particles
  for(i = 0; i < hf->halos[0].num_p; i++)
    hf->halos[0].particle_ids[i] = s->particles[i].id;
}


AfterEach(fill_init_volume)
{
  dispose_halofinder(&hf);
  dispose_snapshot(&s);
}


Ensure(fill_init_volume, counts_number_particle_in_each_grid_properly)
{
  fill_init_volume(hf, s, dims);

  grid_info *grid = vector_get(hf->halos[0].init_volume, 0);

  assert_that(grid, is_non_null);
  assert_that(grid->index, is_equal_to(0));
  assert_that(grid->nParts, is_equal_to(NUMPARTS));
}
