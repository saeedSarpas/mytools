/*
 * ll/ll_traverse_test.c
 * test_for: ll/ll_traverse.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./ll_traverse.h"
#include "./ll_add.h"


#define NUM_OF_NODES 11
#define DOUBLEKEY 1.234


void increment(llnode*, void*);


Describe(ll_traverse);
BeforeEach(ll_traverse) {}
AfterEach(ll_traverse) {}


Ensure(ll_traverse, adds_new_nodes_in_front_of_the_ll_properly)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_addfront(linkedlist, &i, NULL);

  int counter = 0;
  ll_traverse(linkedlist, increment, &counter);

  assert_that(counter, is_equal_to(linkedlist->len));
  dispose_ll(&linkedlist);
}


void increment(llnode *node, void *data)
{
  if(node == NULL)
    return;
  else
    (*(int*)data)++;
}
