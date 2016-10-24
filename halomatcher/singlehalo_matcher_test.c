/*
 * halomatcher/singlehalo_matcher_test.c
 * test_for: halomatcher/singlehalo_matcher.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "singlehalo_matcher.h"
#include "./../halofinder/halofinder.h"
#include "./../avltree/avl_tree.h"
#include "./../memory/allocate.h"


Describe(singlehalo_matcher);


#define PRIHALOID 0
#define NUMPRIHALOPARTS 17
#define NUMSECHALOS 11
#define NUMSECHALOPARTS 17


halo *prihalo;
halofinder *sec;


BeforeEach(singlehalo_matcher)
{
  int i, j;

  prihalo = allocate(1, sizeof(*prihalo));
  prihalo->id = PRIHALOID;
  prihalo->m = 1000;
  for(j = 0; j < 3; j++)
    prihalo->pos[j] = 1.234;
  for(j = 0; j < NUMPRIHALOPARTS; j++)
    prihalo->init_volume = avl_insert(
      prihalo->init_volume, PRIHALOID + j, NULL);

  sec = new_halofinder(NUMSECHALOS);
  for(i = 0; i < sec->header->num_halos; i++){
    sec->halos[i].id = i;
    sec->halos[i].m = 1000;
    for(j = 0; j < 3; j++)
      sec->halos[i].pos[j] = 1.234;
    for(j = 0; j < NUMSECHALOPARTS; j++)
      sec->halos[i].init_volume = avl_insert(
        sec->halos[i].init_volume, i+j, NULL);
  }
}


AfterEach(singlehalo_matcher)
{
  free(prihalo);
  dispose_halofinder(sec);
}


Ensure(singlehalo_matcher, finds_matching_halos_properly)
{
  halomatcher_params p = {.massOffset = 5,
                          .minGoodness = 65.0,
                          .maxDisplacement = 5.0,
                          .loadMatches = 0,
                          .loadMatchesPath = "",
                          .saveMatches = 0,
                          .saveMatchesPath = ""};

  vector *matches_v = singlehalo_matcher(prihalo, sec, p);

  unsigned int i;
  match *dummy_match;
  for(i = 0; i < matches_v->log_length; i++){
    dummy_match = vector_get_elem(matches_v, i);
    assert_that(dummy_match->matchid, is_equal_to(i));
    if(i == PRIHALOID)
      assert_that(dummy_match->goodness, is_equal_to(100));
    else
      assert_that(dummy_match->goodness < 100);
  }
}
