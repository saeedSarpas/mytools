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
#include "./../vector/vector.h"
#include "./../vector/vector_push.h"


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
  grid_info grid;

  pri = new_halofinder(NUMPRIHALOS);
  for(i = 0; i < pri->header->num_halos; i++){
    pri->halos[i].id = i;
    pri->halos[i].m = 1000;
    pri->halos[i].num_p = NUMPRIHALOPARTS;

    for(j = 0; j < 3; j++)
      pri->halos[i].pos[j] = 1.234;

    pri->halos[i].init_volume = new_vector(NUMPRIHALOPARTS, sizeof(grid_info));
    for(j = 0; j < NUMPRIHALOPARTS; j++){
      grid.index = i+j;
      grid.nParts = 1;
      vector_push(pri->halos[i].init_volume, &grid);
    }
  }

  sec = new_halofinder(NUMSECHALOS);
  for(i = 0; i < sec->header->num_halos; i++){
    sec->halos[i].id = i;
    sec->halos[i].m = 1000;
    sec->halos[i].num_p = NUMSECHALOPARTS;

    for(j = 0; j < 3; j++)
      sec->halos[i].pos[j] = 1.234;

    sec->halos[i].init_volume = new_vector(NUMSECHALOPARTS, sizeof(grid_info));
    for(j = 0; j < NUMSECHALOPARTS; j++){
      grid.index = i+j;
      grid.nParts = 1;
      vector_push(sec->halos[i].init_volume, &grid);
    }
  }
}


AfterEach(halomatcher)
{
  dispose_halofinder(&pri);
  dispose_halofinder(&sec);
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


Ensure(halomatcher, finds_matching_halos_in_case_of_multiple_matches)
{
  halomatcher_params p = {.massOffset = 5,
                          .maxDisplacement = 5.0,
                          .loadMatches = 0,
                          .loadMatchesPath = "",
                          .saveMatches = 0,
                          .saveMatchesPath = ""};

  int i, j;
  grid_info grid;

  for(i = 0; i < sec->header->num_halos; i++){
    for(j = 0; j < 3; j++)
      sec->halos[i].pos[j] = i * 1.234;

    dispose_vector(&sec->halos[i].init_volume);
    sec->halos[i].init_volume = new_vector(1, sizeof(grid_info));
    grid.index = 1;
    grid.nParts = NUMSECHALOPARTS;
    vector_push(sec->halos[i].init_volume, &grid);
  }

  for(i = 0; i < pri->header->num_halos; i++){
    dispose_vector(&pri->halos[i].init_volume);
    pri->halos[i].init_volume = new_vector(1, sizeof(grid));
    grid.index = 1;
    grid.nParts = NUMPRIHALOPARTS;
    vector_push(pri->halos[i].init_volume, &grid);
  }

  matchinghalo *mh = halomatcher(pri, sec, p);

  for(i = 0; i < pri->header->num_halos; i++){
    assert_that(mh->matchingids[i], is_equal_to(1));
    assert_that_double(mh->goodnesses[i], is_equal_to_double(100.0));
  }
}
