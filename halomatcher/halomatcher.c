/*
 * halomatcher/halomatcher.c
 * test_file: halomatcher/halomatcher_test.c
 *
 * Finding the matching halos of two given list of halos
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include "halomatcher.h"
#include "./../memory/allocate.h"
#include "./../search/half_interval/find_ge.h"
#include "./../avltree/avl_tree.h"
#include "./../vector/vector.h"
#include "./../loading/simple_loading.h"


static vector* single_halomatcher(halo*, halofinder*, halomatcher_params);
static int compare_mass(const void*, int, const void*);
static int compare_foundmatches(const void*, const void*);
static void reset_counter();
static void increment();
static void fill_v_indices(avl_node*);
static int counter;
static int64_t *v_indices;


typedef struct _foundmatch
{
  int id;
  float goodness;
} foundmatch;


/*
 * Compating two sets of halos with each other to find matches
 *
 * param: pri_halos_msorted primary list of halos which is sorted based on their
 * masses
 * param: sec_halos_msorted secondary list of halos which is sorted based on
 * their masses
 *
 * return: a matchinghalo structure of arrays containing the matching halos
 * between pri/sec_halos_msorted
 */
matchinghalo* halomatcher(halofinder *pri_halos_msorted,
                          halofinder *sec_halos_msorted,
                          halomatcher_params params)
{
  vector **pri_matches = allocate(pri_halos_msorted->header->num_halos,
                                  sizeof(vector*));
  vector **sec_matches = allocate(sec_halos_msorted->header->num_halos,
                                  sizeof(vector*));

  int i, percent = -1;
  // Searching for matches based on the first list of halos
  printf("\tChecking the 1st list of halos:\n");
  for(i = 0; i < pri_halos_msorted->header->num_halos; i++){
    percent = simple_loading(percent, i, pri_halos_msorted->header->num_halos);
    pri_matches[i] = single_halomatcher(&pri_halos_msorted->halos[i],
                                        sec_halos_msorted, params);
  }

  // Searching for matches based on the second list of halos
  printf("\tChecking the 2nd list of halos:\n");
  percent = -1;
  for(i = 0; i < sec_halos_msorted->header->num_halos; i++){
    percent = simple_loading(percent, i, pri_halos_msorted->header->num_halos);
    sec_matches[i] = single_halomatcher(&sec_halos_msorted->halos[i],
                                        pri_halos_msorted, params);
  }
  printf(" [done]\n");


  matchinghalo *mh = new_matchinghalo(pri_halos_msorted->header->num_halos);
  foundmatch *pri_match, *sec_match;

  for(i = 0; i < pri_halos_msorted->header->num_halos; i++){
    if(pri_matches[i]->log_length == 0) continue;
    pri_match = (foundmatch*)vector_get_elem(pri_matches[i], 0);

    if(sec_matches[pri_match->id]->log_length == 0) continue;
    sec_match = (foundmatch*)vector_get_elem(sec_matches[pri_match->id], 0);

    if(sec_match->id == i){
      mh->matchingids[i] = sec_match->id;
      mh->goodnesses[i] = (pri_match->goodness + sec_match->goodness) / 2;
    }
  }

  free(pri_matches);
  free(sec_matches);

  return mh;
}


/*
 * Comparing one halo with an array of halos
 *
 * param: h the refrence halo
 * param: hf a halofinder structure containing an array of halos for comparison
 */
static vector* single_halomatcher(halo *h, halofinder *hf,
                                  halomatcher_params params)
{
  // Calculating target mass range in the secondary halos
  float min_mass = h->m / params.massoffset;
  float max_mass = h->m * params.massoffset;

  // Finding the index of the halo with minimum mass in the secaondary halos
  int min_index = find_ge(&min_mass, hf->halos, hf->header->num_halos,
                          compare_mass);

  if(min_index == NOT_FOUND) return vector_new(0, sizeof(foundmatch));

  // Allocating memory for v_indices (an array containing init_volume
  // indices of the primary halo)
  reset_counter();
  inorder_traversal(h->init_volume, increment);
  int len_v_indices = counter;
  v_indices = allocate(len_v_indices, sizeof(int64_t));

  // Filling v_indices array
  reset_counter();
  inorder_traversal(h->init_volume, fill_v_indices);

  // Creating a vector for saving matching halos
  vector *matches = vector_new(8, sizeof(foundmatch));

  // Finding matching halos in the secaondary halos
  int i, j, nmatching_grids = 0;
  float matchgoodness;
  avl_node *found_node;
  foundmatch matchholder;
  for(i = min_index; i < hf->header->num_halos && hf->halos[i].m < max_mass;
      i++){

    nmatching_grids = 0;

    // Iteration through v_indices array for finding matching volume grids
    for(j = 0; j < len_v_indices; j++){
      found_node = avl_find(hf->halos[i].init_volume, v_indices[j]);
      if(found_node != NULL) nmatching_grids++;
    }

    matchgoodness = (float) nmatching_grids / len_v_indices * 100;

    if(matchgoodness > params.mingoodness){
      matchholder.id = i;
      matchholder.goodness = matchgoodness;
      vector_push(matches, &matchholder);
    }
  }

  qsort(matches->elems, matches->log_length, sizeof(foundmatch),
        compare_foundmatches);

  free(v_indices);
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
 * Compare foundmatches for sorting foundmatches arrays in a decsending order
 */
static int compare_foundmatches(const void *fm1, const void *fm2)
{
  const float g1 = ((foundmatch*)fm1)->goodness;
  const float g2 = ((foundmatch*)fm2)->goodness;

  if(g1 > g2) return -1;
  else if(g1 < g2) return +1;
  else return 0;
}


/*
 * Putting init_volume indices into v_indices array
 */
static void fill_v_indices(avl_node *node)
{
  v_indices[counter] = node->key;
  counter++;
}


/*
 * Counter incrementor
 */
static void increment()
{
  counter += 1;
}


/*
 * Reset counter to zero
 */
static void reset_counter()
{
  counter = 0;
}
