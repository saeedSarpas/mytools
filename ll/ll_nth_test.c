/*
 * ll/ll_nth_test.c
 * test_for: ll/ll_nth.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./ll_nth.h"
#include "./ll_add.h"


#define NUM_OF_NODES 11
#define DOUBLEKEY 1.234


Describe(ll_nth);
BeforeEach(ll_nth) {}
AfterEach(ll_nth) {}


Ensure(ll_nth, returns_null_if_the_n_is_out_of_the_boundary)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_addback(linkedlist, &i, NULL);

  llnode *node = NULL;
  node = ll_nth(linkedlist, NUM_OF_NODES);
  assert_that(node, is_null);

  node = ll_nth(linkedlist, -NUM_OF_NODES - 1);
  assert_that(node, is_null);

  dispose_ll(&linkedlist);
}


Ensure(ll_nth, returns_null_if_the_linkedlist_is_empty)
{
  ll *linkedlist = new_ll(LL_INT);

  llnode *node = ll_nth(linkedlist, 1);
  assert_that(node, is_null);

  dispose_ll(&linkedlist);
}


Ensure(ll_nth, retunrs_the_right_node)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_addback(linkedlist, &i, NULL);

  llnode *node = NULL;

  for(i = 0; i < NUM_OF_NODES; i++){
    node = ll_nth(linkedlist, i);
    assert_that(node->ikey, is_equal_to(i));
  }

  dispose_ll(&linkedlist);
}


Ensure(ll_nth, works_with_negative_n)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_addback(linkedlist, &i, NULL);

  llnode *node = NULL;
  for(i = -1; i >= -NUM_OF_NODES; i--){
    node = ll_nth(linkedlist, i);
    assert_that(node->ikey, is_equal_to(i + NUM_OF_NODES));
  }

  dispose_ll(&linkedlist);
}
