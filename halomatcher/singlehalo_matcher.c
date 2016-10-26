/*
 * halomatcher/singlehalo_matcher.c
 * test_file: halomatcher/singlehalo_matcher_test.c
 *
 * Finding the matching halos of a given halo by comparing it to a list of halos
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include <stdio.h>
#include "singlehalo_matcher.h"
#include "./../search/half_interval/find_ge.h"
#include "./../avltree/avl_tree.h"
#include "./../memory/allocate.h"


static int compare_mass(const void*, int, const void*);
static int compare_matches(const void*, const void*);
static void reset_global_counter();
static void increment();
static void fill_grid_indices(avl_node*);
static int64_t *grid_indices;
static int *grid_nparts;


static int counter;


/*
 * Comparing one halo with an array of halos
 *
 * param: h the refrence halo
 * param: hf a halofinder structure containing an array of halos for comparison
 */
vector* singlehalo_matcher(halo *h, halofinder *hf, halomatcher_params params)
{
  // Calculating target mass range in the secondary halos
  float min_mass = h->m / params.massOffset;
  float max_mass = h->m * params.massOffset;

  // Finding the index of the halo with minimum mass in the secaondary halos
  int min_index = find_ge(&min_mass, hf->halos, hf->header->num_halos,
                          compare_mass);

  if(min_index == NOT_FOUND) return vector_new(0, sizeof(match));

  // Allocating memory for grid_indices (an array containing init_volume
  // indices of the primary halo)
  reset_global_counter();
  inorder_traversal(h->init_volume, increment);
  int len_grid_indices = counter;
  grid_indices = allocate(len_grid_indices, sizeof(int64_t));
  grid_nparts = allocate(len_grid_indices, sizeof(int));

  // Filling grid_indices and grid_nparts arrays
  reset_global_counter();
  inorder_traversal(h->init_volume, fill_grid_indices);

  // Counting the total number of particles in grid_nparts
  int i, ntot_gridparts = 0;
  for(i = 0; i < len_grid_indices; i++)
    ntot_gridparts += grid_nparts[i];

  // Creating a vector for saving matching halos
  vector *matches = vector_new(8, sizeof(match));

  // Finding matching halos in the secaondary halos
  int j, nmatching_grids = 0;
  float matchgoodness, dx2[3];
  avl_node *found_node;
  match matchholder;
  for(i = min_index; i < hf->header->num_halos && hf->halos[i].id != HALONOTSET
        && hf->halos[i].m < max_mass; i++){

    // Check if the halo exceed the maximum amount of displacement
    for(j = 0; j < 3; j++){
      dx2[j] = pow((h->pos[j] - hf->halos[i].pos[j]), 2);
      if(dx2[j] > hf->header->box_size[j] / 2)
        dx2[j] -= hf->header->box_size[j] / 2;
    }

    if(sqrt(dx2[0] + dx2[1] + dx2[2]) > params.maxDisplacement) continue;

    nmatching_grids = 0;

    // Iteration through grid_indices array for finding matching volume grids
    for(j = 0; j < len_grid_indices; j++){
      found_node = avl_find(hf->halos[i].init_volume, grid_indices[j]);
      if(found_node != NULL) nmatching_grids += grid_nparts[j];
    }

    matchgoodness = (float) nmatching_grids / ntot_gridparts * 100;

    if(matchgoodness > 0.0){
      matchholder.matchid = i;
      matchholder.goodness = matchgoodness;
      vector_push(matches, &matchholder);
    }
  }

  qsort(matches->elems, matches->log_length, sizeof(match),
        compare_matches);

  free(grid_indices);
  return matches;
}



/*
 * Comparing float values for using in find_ge function
 */
static int compare_mass(const void *halos, int index, const void *target_mass)
{
  const float halo_mass = ((halo*)halos)[index].m;
  const float t_mass = *(float*)target_mass;

  if(halo_mass < t_mass) return LESS_THAN_T_VALUE;
  else if(halo_mass > t_mass) return GREATER_THAN_T_VALUE;
  else return EQUAL_TO_T_VALUE;
}


/*
 * Compare matches for sorting matches arrays in a decsending order
 */
static int compare_matches(const void *fm1, const void *fm2)
{
  const float g1 = ((match*)fm1)->goodness;
  const float g2 = ((match*)fm2)->goodness;

  if(g1 > g2) return -1;
  else if(g1 < g2) return +1;
  else return 0;
}



/*
 * Reset counter to zero
 */
static void reset_global_counter()
{
  counter = 0;
}



/*
 * Putting init_volume indices into grid_indices array
 */
static void fill_grid_indices(avl_node *node)
{
  grid_indices[counter] = node->key;
  grid_nparts[counter] = *(int*)node->data;
  counter++;
}


/*
 * Counter incrementor
 */
static void increment()
{
  counter += 1;
}