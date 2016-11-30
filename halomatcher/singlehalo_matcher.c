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
#include "./../sort/mergesort/mergesort.h"


static int compare_mass(const void*, int, const void*);
static int compare_matches(void*, int, void*, int);
static void set_matches(void*, int, void*, int);
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
  vector *matches = vector_new(2048, sizeof(match));

  // Finding matching halos in the secaondary halos
  int j, nmatching_gridparts = 0;
  float matchgoodness, dx2[3];
  avl_node *found_node;
  match matchholder;
  for(i = min_index; i < hf->header->num_halos; i++){

    // Checking conditions
    if(hf->halos[i].id == HALONOTSET) continue;
    if(hf->halos[i].m > max_mass) break;

    // Check if the halo exceed the maximum amount of displacement
    for(j = 0; j < 3; j++){
      dx2[j] = pow((h->pos[j] - hf->halos[i].pos[j]), 2);
      if(dx2[j] > hf->header->box_size[j] / 2)
        dx2[j] -= hf->header->box_size[j] / 2;
    }

    if(sqrt(dx2[0] + dx2[1] + dx2[2]) > params.maxDisplacement) continue;

    nmatching_gridparts = 0;

    // Iteration through grid_indices array for finding matching volume grids
    for(j = 0; j < len_grid_indices; j++){
      found_node = avl_find(hf->halos[i].init_volume, grid_indices[j]);
      if(found_node != NULL) nmatching_gridparts += grid_nparts[j];
    }

    matchgoodness = (float) nmatching_gridparts / ntot_gridparts * 100;

    if(matchgoodness > 0.0){
      matchholder.matchid = i;
      matchholder.goodness = matchgoodness;
      vector_push(matches, &matchholder);
    }
  }

  mergesort(matches->elems, 0, matches->log_length - 1, set_matches,
            compare_matches, sizeof(match));

  free(grid_indices);
  free(grid_nparts);
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
static int compare_matches(void *arr1, int index1, void *arr2, int index2)
{
  match* m1 = (match*)((char*)arr1 + index1 * sizeof(match));
  match* m2 = (match*)((char*)arr2 + index2 * sizeof(match));

  if(m1->goodness > m2->goodness)
    return MS_LESS_THAN;
  else if(m1->goodness < m2->goodness)
    return MS_GREATER_THAN;
  else
    return MS_EQUALS;
}



/*
 * Set matches for sorting matches arrays in a decsending order
 */
static void set_matches(void *target_arr, int target_index, void *arr, int index)
{
  match* m1 = (match*)((char*)target_arr + target_index * sizeof(match));
  match* m2 = (match*)((char*)arr + index * sizeof(match));

  *m1 = *m2;
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
