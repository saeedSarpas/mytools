/*
 * halofinder/halofinder.c
 * test_file: halofinder/halofinder_test.c
 *
 * creating and destroying generic halofinder structure
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "halofinder.h"
#include "./../memory/allocate.h"
#include "./../avltree/avl_tree.h"


static void dispose_halofinder(halofinder*);


/*
 * creating a new halofinder structure
 *
 * param: num_halos number of halos
 *
 * return: allocated halofinder structure
 */
halofinder* new_halofinder(int num_halos)
{
  halofinder *hf = allocate(1, sizeof(halofinder));
  hf->header = allocate(1, sizeof(haloheader));
  hf->header->num_halos = num_halos;
  hf->halos = allocate(num_halos, sizeof(halo));

  int i;
  for(i = 0; i < hf->header->num_halos; i++){
    hf->halos[i].init_volume = NULL;
    hf->halos[i].particle_ids = NULL;
  }

  hf->dispose = dispose_halofinder;

  return hf;
}


/*
 * allocating memory for particle ids array
 *
 * param: h the halo that the array should be allocated for that
 * param: num_p number of particles of the halo
 */
void allocate_particle_ids(halo *h, int num_p)
{
  h->particle_ids = allocate(num_p, sizeof(int64_t));
}


/*
 * disposing a given halofinder structure
 *
 * NOTE: memory leak since we don't free hf itself
 *
 * TODO: creating a separate module for managing halo related actions (e.g.
 * creating, destroying, etc.)
 */
static void dispose_halofinder(halofinder *hf)
{
  int i;
  for(i = 0; i < hf->header->num_halos; i++){
    if(hf->halos[i].particle_ids != NULL){
      free(hf->halos[i].particle_ids);
      hf->halos[i].particle_ids = NULL;
    }
    if(hf->halos[i].init_volume != NULL){
      avl_dispose(hf->halos[i].init_volume);
      hf->halos[i].init_volume = NULL;
    }
  }

  free(hf->header);
  hf->header = NULL;

  free(hf->halos);
  hf->halos = NULL;
}
