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
#include "./../loading/simple_loading.h"
#include "./../time/mytime.h"


static float max_goodness(vector*);
static vector* fill_goodest_matches(vector*, float);
static void put_first_match_into_mh(vector*, matchinghalo*, int);
static float min_distance(vector*, halofinder*, int, halofinder*);
static float two_halos_distance(halo*, halo*);


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
  vector **primatches = allocate(pri->header->num_halos, sizeof(vector*));
  vector **secmatches = allocate(sec->header->num_halos, sizeof(vector*));

  unsigned int i, j;

  if(params.loadMatches)
    load_matches(params.loadMatchesPath, primatches, secmatches);
  else {
    /* Searching for matches based on the first list of halos */
    int progress = -1;
    int ntot_halos = pri->header->num_halos + sec->header->num_halos - 1;
    for(i = 0; i < pri->header->num_halos; i++){
      progress = simple_loading(progress, i, ntot_halos);
      if(pri->halos[i].id != HALONOTSET)
        primatches[i] = singlehalo_matcher(&pri->halos[i], sec, params);
    }

    /* Searching for matches based on the second list of halos */
    for(i = 0; i < sec->header->num_halos; i++){
      progress = simple_loading(progress, i+pri->header->num_halos, ntot_halos);
      if(sec->halos[i].id != HALONOTSET)
        secmatches[i] = singlehalo_matcher(&sec->halos[i], pri, params);
    }
  }

  if(params.saveMatches && !params.loadMatches){
    clock_t _s_m_ = start("\tSaving found matches to file");
    write_matches(params.saveMatchesPath, primatches, secmatches,
                  pri->header->num_halos, sec->header->num_halos);
    done(_s_m_);
  }

  matchinghalo *mh = new_matchinghalo(pri->header->num_halos);
  match *primatch, *secmatch;

  int primatch_id;
  match *matchholder = allocate(1, sizeof(match));
  float max_gn, min_dist, dist;
  vector *primatch_matches = NULL;
  //Best matches based on goodness
  vector *goodest_matches = NULL;

  for(primatch_id = 0; primatch_id < pri->header->num_halos; primatch_id++){
    if(primatches[primatch_id]->log_length == 0) continue;

    primatch_matches = vector_new(8, sizeof(match));

    for(i = 0; i < primatches[primatch_id]->log_length; i++){
      primatch = (match*)vector_get_elem(primatches[primatch_id], i);
      if(secmatches[primatch->matchid]->log_length == 0) continue;

      for(j = 0; j < secmatches[primatch->matchid]->log_length; j++){
        secmatch = (match*)vector_get_elem(secmatches[primatch->matchid], j);

        if(secmatch->matchid == primatch_id){
          matchholder->matchid = primatch->matchid;
          matchholder->goodness = (primatch->goodness + secmatch->goodness) / 2;
          vector_push(primatch_matches, matchholder);
        }
      }
    }

    // In case of we only found one match
    if(primatch_matches->log_length == 1)
      put_first_match_into_mh(primatch_matches, mh, primatch_id);
    // If there is more than one match, find the matches with best goodness
    else if(primatch_matches->log_length > 1){
      max_gn = max_goodness(primatch_matches);
      goodest_matches = fill_goodest_matches(primatch_matches, max_gn);
      // If there is only one match with the best goodness value
      if(goodest_matches->log_length == 1)
        put_first_match_into_mh(goodest_matches, mh, primatch_id);
      // If there is more than one "best" match, check the distances and
      // consider the closest match as the real one
      else if(goodest_matches->log_length > 1){
        min_dist = min_distance(goodest_matches, pri, primatch_id, sec);
        for(i = 0; i < goodest_matches->log_length; i++){
          matchholder = vector_get_elem(goodest_matches, i);
          dist = two_halos_distance(&pri->halos[primatch_id],
                                    &sec->halos[matchholder->matchid]);
          if(dist == min_dist){
            mh->matchingids[primatch_id] = matchholder->matchid;
            mh->goodnesses[primatch_id] = matchholder->goodness;
          }
        }
      }
      free(goodest_matches);
      goodest_matches->log_length = 0;
    }
    vector_dispose(primatch_matches);
  }

  for(i = 0; i < pri->header->num_halos; i++)
    vector_dispose(primatches[i]);

  for(i = 0; i < sec->header->num_halos; i++)
    vector_dispose(secmatches[i]);

  free(primatches);
  free(secmatches);

  return mh;
}


static void put_first_match_into_mh(vector *matches, matchinghalo *mh, int id)
{
  match *matchholder = vector_get_elem(matches, 0);
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
  for(i = 0; i < v->log_length; i++){
    matchholder = (match*)vector_get_elem(v, i);
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

  for(i = 0; i < matches->log_length; i++){
    matchholder = vector_get_elem(matches, i);
    dist = two_halos_distance(&pri->halos[id],
                              &sec->halos[matchholder->matchid]);

    if(dist < min_dist)
      min_dist = dist;
  }

  return min_dist;
}


/*
 * Put goodest (best) matches based on goodness in a vector
 */
static vector* fill_goodest_matches(vector *matches, float max_gn)
{
  match *matchholder = NULL;
  vector *v_matches = vector_new(8, sizeof(match));

  unsigned int i;
  for(i = 0; i < matches->log_length; i++){
    matchholder = vector_get_elem(matches, i);
    if(matchholder->goodness == max_gn)
      vector_push(v_matches, matchholder);
  }

  return v_matches;
}


/*
 * Calculating the distance between two halos
 */
static float two_halos_distance(halo *pri, halo *sec)
{
  float dist;

  dist = sqrt(pow(pri->pos[0] - sec->pos[0], 2) +
              pow(pri->pos[1] - sec->pos[1], 2) +
              pow(pri->pos[2] - sec->pos[2], 2));

  return dist;
}
