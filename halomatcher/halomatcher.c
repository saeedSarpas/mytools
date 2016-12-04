/*
 * halomatcher/halomatcher.c
 * test_file: halomatcher/halomatcher_test.c
 *
 * Finding the matching halos of two given list of halos
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <math.h>
#include "halomatcher.h"
#include "load_save_matches.h"
#include "./../memory/allocate.h"
#include "./../vector/vector.h"
#include "./../vector/vector_get.h"
#include "./../vector/vector_push.h"
#include "./../loading/simple_loading.h"
#include "./../time/mytime.h"


static float max_goodness(vector*);
static vector* fill_goodest_matches(vector*, float);
static void put_first_match_into_mh(vector*, matchinghalo*, int);
static float min_distance(vector*, halofinder*, int, halofinder*);
static float two_halos_distance(halo*, halo*, float);


/*
 * Compating two sets of halos with each other to find matches
 *
 * param: pri primary list of halos which is sorted based on their
 * masses
 * param: sec secondary list of halos which is sorted based on
 * their masses
 *
 * return: a matchinghalo structure of arrays containing the matching halos
 * between pri/sec lists
 */
matchinghalo* halomatcher(halofinder *pri, halofinder *sec,
                          halomatcher_params params)
{
  unsigned int i, j;

  vector **primatches_v = allocate(pri->header->num_halos, sizeof(vector*));
  vector **secmatches_v = allocate(sec->header->num_halos, sizeof(vector*));
  for(i = 0; i < sec->header->num_halos; i++)
    secmatches_v[i] = new_vector(0, sizeof(0));

  if(params.loadMatches)
    load_matches(params.loadMatchesPath, primatches_v, secmatches_v);
  else {
    int progress = -1;
    int ntot_halos = pri->header->num_halos - 1;

    /* Searching for matches based on the first list of halos */
    for(i = 0; i < pri->header->num_halos; i++){
      progress = simple_loading(progress, i, ntot_halos);
      if(pri->halos[i].id != HALONOTSET)
        primatches_v[pri->halos[i].id] = singlehalo_matcher(
          &pri->halos[i], sec, secmatches_v, params);
    }
  }

  if(params.saveMatches && !params.loadMatches){
    clock_t _s_m_ = start("\tSaving found matches to file");
    write_matches(params.saveMatchesPath, primatches_v, secmatches_v,
                  pri->header->num_halos, sec->header->num_halos);
    done(_s_m_);
  }

  matchinghalo *mh = new_matchinghalo(pri->header->num_halos);

  match *primatch, *secmatch;
  match *matchholder = allocate(1, sizeof(*matchholder));

  int primatch_id;

  float max_gn, min_dist, dist;


  for(primatch_id = 0; primatch_id < pri->header->num_halos; primatch_id++){
    if(primatches_v[primatch_id]->logLength == 0) continue;

    vector *primatch_matches = new_vector(8, sizeof(match));

    for(i = 0; i < primatches_v[primatch_id]->logLength; i++){
      primatch = (match*)vector_get(primatches_v[primatch_id], i);
      if(secmatches_v[primatch->matchid] == NULL) continue;
      if(secmatches_v[primatch->matchid]->logLength == 0) continue;

      for(j = 0; j < secmatches_v[primatch->matchid]->logLength; j++){
        secmatch = (match*)vector_get(secmatches_v[primatch->matchid], j);

        if(secmatch->matchid == primatch_id){
          matchholder->matchid = primatch->matchid;
          matchholder->goodness = (primatch->goodness + secmatch->goodness) / 2;
          vector_push(primatch_matches, matchholder);
        }
      }
    }

    // In case of we only found one match
    if(primatch_matches->logLength == 1)
      put_first_match_into_mh(primatch_matches, mh, primatch_id);
    // If there is more than one match, find the matches with best goodness
    else if(primatch_matches->logLength > 1){
      max_gn = max_goodness(primatch_matches);
      //Best matches based on goodness
      vector *goodest_matches = fill_goodest_matches(primatch_matches, max_gn);
      // If there is only one match with the best goodness value
      if(goodest_matches->logLength == 1)
        put_first_match_into_mh(goodest_matches, mh, primatch_id);
      // If there is more than one "best" match, check the distances and
      // consider the closest match as the real one
      else if(goodest_matches->logLength > 1){
        min_dist = min_distance(goodest_matches, pri, primatch_id, sec);
        for(i = 0; i < goodest_matches->logLength; i++){
          matchholder = vector_get(goodest_matches, i);
          dist = two_halos_distance(&pri->halos[primatch_id],
                                    &sec->halos[matchholder->matchid],
                                    pri->header->box_size[0]);
          if(dist == min_dist){
            mh->matchingids[primatch_id] = matchholder->matchid;
            mh->goodnesses[primatch_id] = matchholder->goodness;
          }
        }
      }
      dispose_vector(&goodest_matches);
    }
    dispose_vector(&primatch_matches);
  }

  for(i = 0; i < pri->header->num_halos; i++)
    dispose_vector(&primatches_v[i]);

  for(i = 0; i < sec->header->num_halos; i++)
    if(secmatches_v[i] != NULL)
      dispose_vector(&secmatches_v[i]);

  free(primatches_v);
  free(secmatches_v);

  return mh;
}


static void put_first_match_into_mh(vector *matches, matchinghalo *mh, int id)
{
  match *matchholder = vector_get(matches, 0);
  mh->matchingids[id] = matchholder->matchid;
  mh->goodnesses[id] = matchholder->goodness;
}

/*
 * Finding the maximum of goodness in a vector of matches
 */
static float max_goodness(vector *v)
{
  match *matchholder = NULL;

  unsigned int i;
  float max_gn = 0.0;
  for(i = 0; i < v->logLength; i++){
    matchholder = (match*)vector_get(v, i);
    if(matchholder->goodness > max_gn)
      max_gn = matchholder->goodness;
  }

  return max_gn;
}


/*
 * Finding the maximum distance of a series of matches
 */
static float min_distance(vector *matches, halofinder *pri, int id,
                          halofinder *sec)
{
  float dist, min_dist = pri->header->box_size[0];

  unsigned int i;
  match *matchholder = NULL;

  for(i = 0; i < matches->logLength; i++){
    matchholder = vector_get(matches, i);
    dist = two_halos_distance(&pri->halos[id],
                              &sec->halos[matchholder->matchid],
                              pri->header->box_size[0]);

    if(dist < min_dist) min_dist = dist;
  }

  return min_dist;
}


/*
 * Put goodest (best) matches based on goodness in a vector
 */
static vector* fill_goodest_matches(vector *matches, float max_gn)
{
  match *matchholder = NULL;
  vector *v_matches = new_vector(8, sizeof(match));

  unsigned int i;
  for(i = 0; i < matches->logLength; i++){
    matchholder = vector_get(matches, i);
    if(matchholder->goodness == max_gn)
      vector_push(v_matches, matchholder);
  }

  return v_matches;
}


/*
 * Calculating the distance between two halos
 */
static float two_halos_distance(halo *pri, halo *sec, float box_length)
{
  float dx[3];
  int i;
  for(i = 0; i < 3; i++){
    dx[i] = fabs(pri->pos[i] - sec->pos[i]);
    if(dx[i] > box_length / 2) dx[i] -= box_length / 2;
  }

  return sqrt(pow(dx[0], 2) + pow(dx[1], 2) + pow(dx[2], 2));
}
