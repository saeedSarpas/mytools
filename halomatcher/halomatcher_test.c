/*
 * halomatcher/halomatcher_test.c
 * test_for: halomatcher/halomatcher.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "halomatcher.h"
#include "singlehalo_matcher.h"
#include "./../halofinder/halofinder.h"
#include "./../avltree/avl_tree.h"


Describe(halomatcher);


#define NUMPRIHALOS 11
#define NUMPRIHALOPARTS 17
#define NUMSECHALOS 11
#define NUMSECHALOPARTS 17


halofinder *pri;
halofinder *sec;


BeforeEach(halomatcher)
{
  int i, j;

  pri = new_halofinder(NUMPRIHALOS);
  for(i = 0; i < pri->header->num_halos; i++){
    pri->halos[i].id = i;
    pri->halos[i].m = 1000;
    for(j = 0; j < 3; j++)
      pri->halos[i].pos[j] = 1.234;
    for(j = 0; j < NUMPRIHALOPARTS; j++)
      pri->halos[i].init_volume = avl_insert(
        pri->halos[i].init_volume, i+j, NULL);
  }

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


AfterEach(halomatcher)
{
  dispose_halofinder(pri);
  dispose_halofinder(sec);
}


Ensure(halomatcher, finds_matching_halos_properly)
{
  halomatcher_params p = {.massOffset = 5,
                          .maxDisplacement = 5.0,
                          .loadMatches = 0,
                          .loadMatchesPath = "",
                          .saveMatches = 0,
                          .saveMatchesPath = ""};

  matchinghalo *mh = halomatcher(pri, sec, p);

  int i;
  for(i = 0; i < pri->header->num_halos; i++){
    assert_that(mh->matchingids[i], is_equal_to(i));
    assert_that_double(mh->goodnesses[i], is_equal_to_double(100.0));
  }
}
