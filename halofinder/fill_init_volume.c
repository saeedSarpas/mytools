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
#include "./../memory/allocate.h"


void fill_init_volume(halofinder *hf, snapshot *s, int *dims)
{
  float *pos;
  int i, j, index;

  double *box = allocate(3, sizeof(double));
  for(i = 0; i < 3; i++)
    box[i] = (double) s->header->boxsize;

  int initial_data = 1;
  avl_node *found_node;
  for(i = 0; i < hf->header->num_halos; i++)
    for(j = 0; j < hf->halos[i].num_p; j++){
      pos = s->particles[hf->halos[i].particle_ids[j]].pos;
      index = point_to_grid(pos, box, dims);
      found_node = avl_find(hf->halos[i].init_volume, index);
      if(found_node == NULL)
        hf->halos[i].init_volume = avl_insert(hf->halos[i].init_volume, index,
                                              &initial_data, 1, sizeof(int));
      else
        (*(int*)found_node->data)++;
    }

  return;
}
