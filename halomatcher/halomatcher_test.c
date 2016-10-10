/*
 * halomatcher/halomatcher_test.c
 * test_for: halomatcher/halomatcher.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "halomatcher.h"
#include "./../halofinder/halofinder.h"
#include "./../avltree/avl_tree.h"


Describe(halomatcher);


#define NUMPRIHALOS 11
#define NUMPRIHALOPARTS 17
#define NUMSECHALOS 11
#define NUMSECHALOPARTS 17


halofinder *pri_halos;
halofinder *sec_halos;


BeforeEach(halomatcher)
{
  int i, j;

  pri_halos = new_halofinder(NUMPRIHALOS);
  for(i = 0; i < pri_halos->header->num_halos; i++){
    pri_halos->halos[i].id = i;
    pri_halos->halos[i].m = 1000;
    for(j = 0; j < NUMPRIHALOPARTS; j++)
      pri_halos->halos[i].init_volume = avl_insert(pri_halos->halos[i].init_volume,
                                                  i+j, NULL);
  }

  sec_halos = new_halofinder(NUMSECHALOS);
  for(i = 0; i < sec_halos->header->num_halos; i++){
    sec_halos->halos[i].id = i;
    sec_halos->halos[i].m = 1000;
    for(j = 0; j < NUMSECHALOPARTS; j++)
      sec_halos->halos[i].init_volume = avl_insert(sec_halos->halos[i].init_volume,
                                                  i+j, NULL);
  }

}


AfterEach(halomatcher)
{
  dispose_halofinder(pri_halos);
  dispose_halofinder(sec_halos);
}


Ensure(halomatcher, finds_matching_halos_properly)
{
  halomatcher_params p = {.massoffset = 0.1, .mingoodness = 65.0};
  matchinghalo *mh = halomatcher(pri_halos, sec_halos, p);

  int i;
  for(i = 0; i < pri_halos->header->num_halos; i++){
    assert_that(mh->matchingids[i], is_equal_to(i));
    assert_that_double(mh->goodnesses[i], is_equal_to_double(100.0));
  }
}
