/*
 * halomatcher/singlehalo_matcher.c
 * test_file: halomatcher/singlehalo_matcher_test.c
 *
 * Finding the matching halos of a given halo by comparing it to a list of halos
 *
 * Parameters
 * h: The refrence halo
 * hf: A halofinder structure containing an array of halos for comparison
 * secmatches: An array of vectors
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include <stdio.h>
#include "singlehalo_matcher.h"
#include "./../search/half_interval/find_ge.h"
#include "./../vector/vector_get.h"
#include "./../vector/vector_push.h"
#include "./../memory/allocate.h"


static int compare_mass(const void*, int, const void*);
static void find_common_elements(vector*, vector*, int64_t*, int64_t*);


vector* singlehalo_matcher(halo *h, halofinder *hf, vector **secmatches,
                           halomatcher_params params)
{
  float min_mass = h->m / params.massOffset;
  float max_mass = h->m * params.massOffset;

  int min_index = find_ge(&min_mass, hf->halos, hf->header->num_halos,
                          compare_mass);

  if(min_index == NOT_FOUND) return new_vector(0, sizeof(match));

  unsigned int i, j;
  match matchholder;
  match *dummy_match;
  vector *matches = new_vector(4, sizeof(match));
  float highest_match_gn1 = 1.0, match_gn1 = 0.0, match_gn2 = 0.0, dx[3];
  int64_t common_parts1, common_parts2;
  for(i = min_index; i < hf->header->num_halos; i++){
    if(hf->halos[i].id == HALONOTSET) continue;
    if(hf->halos[i].m > max_mass) break;

    float *box_size = hf->header->box_size;

    for(j = 0; j < 3; j++){
      dx[j] = fabs(h->pos[j] - hf->halos[i].pos[j]);
      if(dx[j] > box_size[j] / 2) dx[j] -= box_size[j] / 2;
    }

    float dist = sqrt(pow(dx[0], 2) + pow(dx[1], 2) + pow(dx[2], 2));
    if(dist > params.maxDisplacement) continue;

    find_common_elements(h->init_volume, hf->halos[i].init_volume,
                         &common_parts1, &common_parts2);

    match_gn1 = (float)common_parts1 / h->num_p * 100;
    match_gn2 = (float)common_parts2 / hf->halos[i].num_p * 100;

    // Primary halo
    if(match_gn1 > highest_match_gn1){
      highest_match_gn1 = match_gn1;
      matches->logLength = 0;

      matchholder.matchid = i;
      matchholder.goodness = match_gn1;
      vector_push(matches, &matchholder);
    }
    else if(match_gn1 == highest_match_gn1){
      matchholder.matchid = i;
      matchholder.goodness = match_gn1;
      vector_push(matches, &matchholder);
    }

    // Secondary halos
    if(match_gn2 > 0){
      if(secmatches[i] == NULL){
        secmatches[i] = new_vector(4, sizeof(match));
        matchholder.matchid = h->id;
        matchholder.goodness = match_gn2;
        vector_push(secmatches[i], &matchholder);
      }
      else {
        dummy_match = vector_get(secmatches[i], 0);

        if(match_gn2 == dummy_match->goodness){
          matchholder.matchid = h->id;
          matchholder.goodness = match_gn2;
          vector_push(secmatches[i], &matchholder);
        }
        else if(match_gn2 > dummy_match->goodness){
          secmatches[i]->logLength = 0;
          matchholder.matchid = h->id;
          matchholder.goodness = match_gn2;
          vector_push(secmatches[i], &matchholder);
        }
      }
    }
  }

  return matches;
}


static int compare_mass(const void *halos, int index, const void *target_mass)
{
  const float halo_mass = ((halo*)halos)[index].m;
  const float t_mass = *(float*)target_mass;

  if(halo_mass < t_mass) return LESS_THAN_T_VALUE;
  else if(halo_mass > t_mass) return GREATER_THAN_T_VALUE;
  else return EQUAL_TO_T_VALUE;
}


static void find_common_elements(vector *v1, vector *v2,
                                 int64_t *ce1, int64_t *ce2)
{
  unsigned int i = 0, j = 0;
  *ce1 = 0;
  *ce2 = 0;
  while(i < v1->logLength && j < v2->logLength){
    if(((grid_info*)vector_get(v1, i))->index >
       ((grid_info*)vector_get(v2, j))->index)
      j++;
    else if(((grid_info*)vector_get(v1, i))->index <
            ((grid_info*)vector_get(v2, j))->index)
      i++;
    else{
      *ce1 += ((grid_info*)vector_get(v1, i))->nParts;
      *ce2 += ((grid_info*)vector_get(v2, i))->nParts;
      i++;
      j++;
    }
  }
}
