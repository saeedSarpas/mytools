/*
 * examples/rg_matchfinder/mh_cascade/mh_cascade.c
 * test_file: examples/rg_matchfinder/mh_cascade/mh_cascade_test.c
 *
 * Generating a cascade of the halo id at z = 0 and it's main progenitor along
 * the different snapshots
 *
 * Parameters
 * halo_id: the id of the halo at z = 0 (or a desired redshift)
 * internal_matches: an array of internal matches between two neighboring
 * snapshot
 * length: the length of internal_matches array
 *
 * Return:
 * a linked list containing the halo ids as the integer key of the list nodes
 *
 * author: Saeed Sarpas
 */


#include "./mh_cascade.h"
#include "./../../../ll/ll_add.h"
#include "./../../../avltree/avl_find.h"


ll* mh_cascade(int halo_id, avltree **internal_matches, int length)
{
  ll *cascade = new_ll(LL_INT);

  ll_addback(cascade, &halo_id, NULL);

  avlnode *node = NULL;
  int i, found_id = halo_id;

  if(internal_matches == NULL)
    return cascade;

  for(i = length - 1; i >= 0; i--){
    node = avl_find(internal_matches[i], &found_id);

    if(node == NULL) break;

    found_id = *(int*)node->data;
    ll_addback(cascade, &found_id, NULL);
  }

  return cascade;
}
