/*
 * halofinder/fill_init_volume_test.c
 * test_for: halofinder/fill_init_volume.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "fill_init_volume.h"
#include "./../halofinder/halofinder.h"
#include "./../avltree/avl_tree.h"
#include "./../memory/allocate.h"
#include "./../grid/row_major_order/point_to_grid.h"


Describe(fill_init_volume);


#define BOXLEN 100
#define NUMHALOS 1
#define NUMPARTS 10
#define PARTMASS 1.2e9
#define GRIDX 100
#define GRIDY 100
#define GRIDZ 100


int dims[3] = {GRIDX, GRIDY, GRIDZ};


BeforeEach(fill_init_volume) {}
AfterEach(fill_init_volume) {}


Ensure(fill_init_volume, works_properly)
{
  // Generating snapshot
  snapshot *s = new_snapshot(NUMPARTS);

  int i;
  for(i = 0; i < 6; i++){
    s->header->npart[i] = 0;
    s->header->mass[i] = 0.0;
  }
  s->header->npart[1] = NUMPARTS;
  s->header->mass[1] = PARTMASS;

  s->header->time = 1.0;
  s->header->redshift = 0.0;

  s->header->boxsize = BOXLEN;

  for(i = 0; i < s->header->tot_nparticles; i++)
    s->particles[i].id = i;

  for(i = 0; i < s->header->tot_nparticles; i++){
    s->particles[i].pos[0] = i * s->header->boxsize / dims[0];
    s->particles[i].pos[1] = 0.0;
    s->particles[i].pos[1] = 0.0;
  }

  // Generating halofinder
  halofinder *hf = new_halofinder(NUMHALOS);

  for(i = 0; i < 3; i++)
    hf->header->box_size[i] = BOXLEN;

  for(i = 0; i < hf->header->num_halos; i++){
    hf->halos[i].id = i;
    hf->halos[i].num_p = NUMPARTS;
    allocate_particle_ids(&hf->halos[i], hf->halos[i].num_p);
  }

  // Filling halo particles
  int index;
  for(i = 0; i < hf->halos[0].num_p; i++)
    hf->halos[0].particle_ids[i] = i;

  fill_init_volume(hf, s, dims);

  float *pos;
  double box_lengths[3] = {s->header->boxsize, s->header->boxsize,
                           s->header->boxsize};
  for(i = 0; i < hf->halos[0].num_p; i++){
    pos = s->particles[hf->halos[0].particle_ids[i]].pos;
    index = point_to_grid(pos, box_lengths, dims);

    avl_node *found_node = avl_find(hf->halos[0].init_volume, index);
    if(found_node == NULL)
      printf("in ke null shod\n\n");
    assert_that(found_node, is_non_null);
  }

  dispose_halofinder(hf);
  dispose_snapshot(s);
}


Ensure(fill_init_volume, counts_number_particle_in_each_grid_properly)
{
  // Generating snapshot
  snapshot *s = new_snapshot(NUMPARTS);

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
  halofinder *hf = new_halofinder(1);

  for(i = 0; i < 3; i++)
    hf->header->box_size[i] = BOXLEN;

  hf->halos[0].id = 0;
  hf->halos[0].num_p = NUMPARTS;
  allocate_particle_ids(&hf->halos[0], hf->halos[0].num_p);

  // Filling halo particles
  for(i = 0; i < hf->halos[0].num_p; i++)
    hf->halos[0].particle_ids[i] = s->particles[i].id;

  fill_init_volume(hf, s, dims);

  avl_node *found_node = avl_find(hf->halos[0].init_volume, 0);

  assert_that(found_node, is_non_null);
  assert_that(found_node->key, is_equal_to(0));
  assert_that(*(int*)found_node->data, is_equal_to(NUMPARTS));
}
