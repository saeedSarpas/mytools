/*
 * examples/rg_matchfinder/mh_cascade/cascade_test.c
 * test_for: examples/rg_matchfinder/mh_cascade/cascade.c
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


Describe(cascade);


BeforeEach(cascade)
{
  internal_matches = allocate(NUM_OF_SNAPS, sizeof(*internal_matches));
  int i, j;
  for(i = 0; i < NUM_OF_SNAPS; i++){
    internal_matches[i] = new_avltree(set_int_key, compare_int_keys);
    j = i + 1;
    avl_insert(internal_matches[i], &j, &i, 1, sizeof(int));
  }
}


AfterEach(cascade)
{
  int i;
  for(i = 0; i < NUM_OF_SNAPS; i++)
    dispose_avltree(&internal_matches[i]);
  free(internal_matches);
}


void check_the_halo_id(llnode*, void*);


Ensure(cascade, returns_the_right_internal_matching_halos)
{
  ll *cascade = mh_cascade(NUM_OF_SNAPS, internal_matches, NUM_OF_SNAPS);

  int id = NUM_OF_SNAPS;
  ll_traverse(cascade, check_the_halo_id, &id);

  dispose_ll(&cascade);
}


void check_the_halo_id(llnode *node, void *data)
{
  assert_that(node->ikey, is_equal_to(*(int*)data));
  *(int*)data -= 1;
}
