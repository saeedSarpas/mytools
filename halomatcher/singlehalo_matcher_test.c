/*
 * halomatcher/singlehalo_matcher_test.c
 * test_for: halomatcher/singlehalo_matcher.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "singlehalo_matcher.h"
#include "./../halofinder/halofinder.h"
#include "./../vector/vector.h"
#include "./../vector/vector_push.h"
#include "./../vector/vector_get.h"
#include "./../memory/allocate.h"


Describe(singlehalo_matcher);


#define PRIHALOID 7
#define NUMPRIHALOPARTS 17
#define NUMSECHALOS 11
#define NUMSECHALOPARTS 17


halo *prihalo;
halofinder *sec;


BeforeEach(singlehalo_matcher)
{
  int i, j;
  grid_info grid;

  prihalo = allocate(1, sizeof(*prihalo));
  prihalo->id = PRIHALOID;
  prihalo->m = 1000;
  prihalo->num_p = NUMPRIHALOPARTS;

  for(j = 0; j < 3; j++)
    prihalo->pos[j] = 1.234;

  prihalo->init_volume = new_vector(NUMPRIHALOPARTS, sizeof(grid_info));
  for(j = 0; j < NUMPRIHALOPARTS; j++){
    grid.index = PRIHALOID + j;
    grid.nParts = 1;
    vector_push(prihalo->init_volume, &grid);
  }

  sec = new_halofinder(NUMSECHALOS);
  sec->header->box_size[0] = 100;
  sec->header->box_size[1] = 100;
  sec->header->box_size[2] = 100;
  for(i = 0; i < sec->header->num_halos; i++){
    sec->halos[i].id = i;
    sec->halos[i].m = 1000;
    sec->halos[i].num_p = NUMSECHALOS;

    for(j = 0; j < 3; j++)
      sec->halos[i].pos[j] = 1.234;

    sec->halos[i].init_volume = new_vector(NUMSECHALOPARTS, sizeof(grid_info));
    for(j = 0; j < NUMSECHALOPARTS; j++){
      grid.index = i + j;
      grid.nParts = 1;
      vector_push(sec->halos[i].init_volume, &grid);
    }
  }
}


AfterEach(singlehalo_matcher)
{
  free(prihalo);
  dispose_halofinder(&sec);
}


Ensure(singlehalo_matcher, finds_matching_halos_properly)
{
  halomatcher_params p = {.massOffset = 5,
                          .maxDisplacement = 5.0,
                          .loadMatches = 0,
                          .loadMatchesPath = "",
                          .saveMatches = 0,
                          .saveMatchesPath = ""};

  int i;
  vector **secmatches = allocate(NUMSECHALOS, sizeof(*secmatches));

  vector *primatch = singlehalo_matcher(prihalo, sec, secmatches, p);

  assert_that(primatch->logLength, is_equal_to(1));

  match *match = vector_get(primatch, 0);
  assert_that(match->matchid, is_equal_to(PRIHALOID));
  assert_that(match->goodness, is_equal_to(100));

  for(i = 0; i < NUMSECHALOS; i++){
    assert_that(secmatches[i]->logLength, is_equal_to(1));
    match = vector_get(secmatches[i], 0);
    assert_that(match->matchid, is_equal_to(prihalo->id));
  }
}
