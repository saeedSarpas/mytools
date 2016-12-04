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
static int fequal(float, float, float);


vector* singlehalo_matcher(halo *h, halofinder *hf, vector **secmatches,
                           halomatcher_params params)
{
  if(h->id == HALONOTSET) return new_vector(0, sizeof(match));

  float min_mass = h->m / params.massOffset;
  float max_mass = h->m * params.massOffset;

  int min_index = find_ge(&min_mass, hf->halos, hf->header->num_halos,
                          compare_mass);

  if(min_index == NOT_FOUND) return new_vector(0, sizeof(match));

  unsigned int i, j, sec_id;
  match matchholder;
  match *saved_match;
  vector *primatches = new_vector(4, sizeof(match));
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

    sec_id = (int)hf->halos[i].id;

    // Primary halo
    if(match_gn1 > highest_match_gn1){
      highest_match_gn1 = match_gn1;

      primatches->logLength = 0;
      matchholder.matchid = sec_id;
      matchholder.goodness = match_gn1;
      vector_push(primatches, &matchholder);
    }
    else if(fequal(match_gn1, highest_match_gn1, 0.1)){
      matchholder.matchid = sec_id;
      matchholder.goodness = match_gn1;
      vector_push(primatches, &matchholder);
    }

    // Secondary halos
    if(match_gn2 > 0){
      if(secmatches[sec_id] == NULL || secmatches[sec_id]->logLength == 0){
        secmatches[sec_id] = new_vector(4, sizeof(match));
        matchholder.matchid = (int)h->id;
        matchholder.goodness = match_gn2;
        vector_push(secmatches[sec_id], &matchholder);
      }
      else {
        saved_match = vector_get(secmatches[sec_id], 0);

        if(fequal(match_gn2, saved_match->goodness, 0.1)){
          matchholder.matchid = (int)h->id;
          matchholder.goodness = match_gn2;
          vector_push(secmatches[sec_id], &matchholder);
        }
        else if(match_gn2 > saved_match->goodness){
          secmatches[sec_id]->logLength = 0;
          matchholder.matchid = (int)h->id;
          matchholder.goodness = match_gn2;
          vector_push(secmatches[sec_id], &matchholder);
        }
      }
    }
  }

  return primatches;
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
                                 int64_t *cp1, int64_t *cp2)
{
  unsigned int i = 0, j = 0;
  *cp1 = 0;
  *cp2 = 0;
  while(i < v1->logLength && j < v2->logLength){
    if(((grid_info*)vector_get(v1, i))->index >
       ((grid_info*)vector_get(v2, j))->index)
      j++;
    else if(((grid_info*)vector_get(v1, i))->index <
            ((grid_info*)vector_get(v2, j))->index)
      i++;
    else{
      *cp1 += ((grid_info*)vector_get(v1, i))->nParts;
      *cp2 += ((grid_info*)vector_get(v2, j))->nParts;
      i++;
      j++;
    }
  }
}


static int fequal(float a, float b, float epsilon)
{
  return fabs(a - b) < epsilon;
}
