/*
 * halomatcher/halomatcher.c
 * test_file: halomatcher/halomatcher_test.c
 *
 * Finding the matching halos of two given list of halos
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "halomatcher.h"
#include "load_save_matches.h"
#include "./../memory/allocate.h"
#include "./../vector/vector.h"
#include "./../loading/simple_loading.h"
#include "./../time/mytime.h"


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

  int i;

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
      progress = simple_loading(progress, i + pri->header->num_halos, ntot_halos);
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

  for(primatch_id = 0; primatch_id < pri->header->num_halos; primatch_id++){
    if(primatches[primatch_id]->log_length == 0) continue;
    primatch = (match*)vector_get_elem(primatches[primatch_id], 0);

    if(secmatches[primatch->matchid]->log_length == 0) continue;
    secmatch = (match*)vector_get_elem(secmatches[primatch->matchid], 0);

    if(secmatch->matchid == primatch_id){
      mh->matchingids[primatch_id] = primatch->matchid;
      mh->goodnesses[primatch_id] = (primatch->goodness + secmatch->goodness) / 2;
    }
  }

  for(i = 0; i < pri->header->num_halos; i++)
    vector_dispose(primatches[i]);

  for(i = 0; i < sec->header->num_halos; i++)
    vector_dispose(secmatches[i]);

  free(primatches);
  free(secmatches);

  return mh;
}
