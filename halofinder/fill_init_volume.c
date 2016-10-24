/*
 * halofinder/fill_init_volume.c
 * test_file: halofinder/fill_init_volume_test.c
 *
 * filling the init_volume member of a halofinder structure
 *
 * author: Saeed Sarpas
 */


#include "fill_init_volume.h"
#include "./../avltree/avl_tree.h"
#include "./../grid/row_major_order/point_to_grid.h"


void fill_init_volume(halofinder *hf, snapshot *s, double *box, int *dims)
{
  float *pos;
  int i, j, index;
  for(i = 0; i < hf->header->num_halos; i++)
    for(j = 0; j < hf->halos[i].num_p; j++){
      pos = s->particles[hf->halos[i].particle_ids[j]].pos;
      index = point_to_grid(pos, box, dims);
      hf->halos[i].init_volume = avl_insert(hf->halos[i].init_volume, index,
                                            NULL);
    }

  return;
}