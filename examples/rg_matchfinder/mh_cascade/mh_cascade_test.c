/*
 * examples/rg_matchfinder/mh_cascade/mh_cascade_test.c
 * test_for: examples/rg_matchfinder/mh_cascade/mh_cascade.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "mh_cascade.h"
#include "./../../../ll/ll.h"
#include "./../../../ll/ll_traverse.h"
#include "./../../../avltree/avl_insert.h"
#include "./../../../memory/allocate.h"


#define NUM_OF_SNAPS 11


avltree **internal_matches = NULL;


Describe(mh_cascade);


BeforeEach(mh_cascade)
{
  internal_matches = allocate(NUM_OF_SNAPS, sizeof(*internal_matches));
  int i, j;
  for(i = 0; i < NUM_OF_SNAPS; i++){
    internal_matches[i] = new_avltree(set_int_key, compare_int_keys);
    j = i + 1;
    avl_insert(internal_matches[i], &j, &i, 1, sizeof(int));
  }
}


AfterEach(mh_cascade)
{
  int i;
  for(i = 0; i < NUM_OF_SNAPS; i++)
    dispose_avltree(&internal_matches[i]);
  free(internal_matches);
}


void check_the_halo_id(llnode*, void*);


Ensure(mh_cascade, returns_the_right_internal_matching_halos)
{
  ll *cascade = mh_cascade(NUM_OF_SNAPS, internal_matches, NUM_OF_SNAPS);

  int id = NUM_OF_SNAPS;
  assert_that(cascade->len, is_equal_to(NUM_OF_SNAPS + 1));
  ll_traverse(cascade, check_the_halo_id, &id);

  dispose_ll(&cascade);
}


Ensure(mh_cascade, handles_properly_NULL_internal_matches)
{
  ll *cascade = mh_cascade(NUM_OF_SNAPS, NULL, NUM_OF_SNAPS);

  assert_that(cascade->len, is_equal_to(1));

  dispose_ll(&cascade);
}


void check_the_halo_id(llnode *node, void *data)
{
  assert_that(node->ikey, is_equal_to(*(int*)data));
  *(int*)data -= 1;
}
