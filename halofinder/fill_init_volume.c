/*
 * halofinder/fill_init_volume.c
 * test_file: halofinder/fill_init_volume_test.c
 *
 * filling the init_volume member of a halofinder structure
 *
 * author: Saeed Sarpas
 */


#include "fill_init_volume.h"
#include "./../avltree/avltree.h"
#include "./../avltree/avl_find.h"
#include "./../avltree/avl_insert.h"
#include "./../avltree/avl_inorder_traversal.h"
#include "./../vector/vector_push.h"
#include "./../grid/row_major_order/point_to_grid.h"
#include "./../memory/allocate.h"


static void fill_init_volume_vector(avlnode*, void*);


void fill_init_volume(halofinder *hf, snapshot *s, int *dims)
{
  int i, j, index;
  float *pos;

  double box[3] = {(double)s->header->boxsize,
                   (double)s->header->boxsize,
                   (double)s->header->boxsize};

  int initial_data = 1, num_grids;

  avlnode *found_node;

  for(i = 0; i < hf->header->num_halos; i++){
    num_grids = 0;
    avltree *tree = new_avltree(set_int_key, compare_int_keys);
    for(j = 0; j < hf->halos[i].num_p; j++){

      pos = s->particles[hf->halos[i].particle_ids[j]].pos;
      index = point_to_grid(pos, box, dims);

      found_node = avl_find(tree, &index);

      if(found_node == NULL){
        avl_insert(tree, &index, &initial_data, 1, sizeof(int));
        num_grids++;
      } else {
        (*(int*)found_node->data)++;
      }
    }

    hf->halos[i].init_volume = new_vector(num_grids + 1, sizeof(grid_info));
    avl_inorder_traversal(tree, fill_init_volume_vector,
                          hf->halos[i].init_volume);

    dispose_avltree(&tree);
  }
}


static void fill_init_volume_vector(avlnode *node, void *v)
{
  grid_info grid = {.index = *(int*)node->key, .nParts = *(int*)node->data};
  vector_push(v, &grid);
}
