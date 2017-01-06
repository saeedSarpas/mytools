/*
 * ll/ll_pop_test.c
 * test_for: ll/ll_pop.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./ll_pop.h"
#include "./ll_add.h"


#define NUM_OF_NODES 11


Describe(ll_pop);
BeforeEach(ll_pop) {}
AfterEach(ll_pop) {}


Ensure(ll_pop, adds_new_nodes_in_front_of_the_ll_properly)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_addback(linkedlist, &i, NULL);

  llnode *node = NULL;

  for(i = 0; i < NUM_OF_NODES; i++){
    assert_that(linkedlist->head->ikey, is_equal_to(i));
    assert_that(linkedlist->len, is_equal_to(NUM_OF_NODES - i));
    node = ll_pop(linkedlist);
    assert_that(node->ikey, is_equal_to(i));
    assert_that(linkedlist->len, is_equal_to(NUM_OF_NODES - i - 1));
    if(linkedlist->head)
      assert_that(linkedlist->head->ikey, is_equal_to(i+1));
  }
}
