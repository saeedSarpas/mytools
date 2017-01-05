/*
 * ll/ll_add_test.c
 * test_for: ll/ll_add.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "ll_add.h"


#define NUM_OF_NODES 11
#define DOUBLEKEY 1.234


Describe(ll_add);
BeforeEach(ll_add) {}
AfterEach(ll_add) {}


Ensure(ll_add, adds_new_nodes_in_front_of_the_ll_properly)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_add(linkedlist, &i, LL_FRONT, NULL);

  assert_that(linkedlist->head->ikey, is_equal_to(NUM_OF_NODES - 1));
  assert_that(linkedlist->len, is_equal_to(NUM_OF_NODES));
  dispose_ll(&linkedlist);
}


Ensure(ll_add, adds_new_nodes_at_the_back_of_the_ll_properly)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_add(linkedlist, &i, LL_BACK, NULL);

  assert_that(linkedlist->head->ikey, is_equal_to(0));
  assert_that(linkedlist->len, is_equal_to(NUM_OF_NODES));
  dispose_ll(&linkedlist);
}


Ensure(ll_add, works_with_double_keys)
{
  ll *linkedlist = new_ll(LL_DOUBLE);

  double key = DOUBLEKEY;
  ll_add(linkedlist, &key, LL_FRONT, NULL);

  assert_that_double(linkedlist->head->dkey, is_equal_to_double(DOUBLEKEY));
  assert_that(linkedlist->len, is_equal_to(1));
  dispose_ll(&linkedlist);
}


Ensure(ll_add, is_able_to_load_the_data)
{
  ll *linkedlist = new_ll(LL_DOUBLE);

  double key = DOUBLEKEY;
  int data = 123;
  ll_add(linkedlist, &key, LL_FRONT, &data, sizeof(int));

  assert_that(*(int*)linkedlist->head->data, is_equal_to(data));
  assert_that(linkedlist->len, is_equal_to(1));
  dispose_ll(&linkedlist);
}


Ensure(ll_add, works_also_as_ll_addfront)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_addfront(linkedlist, &i, NULL);

  assert_that(linkedlist->head->ikey, is_equal_to(NUM_OF_NODES - 1));
  assert_that(linkedlist->len, is_equal_to(NUM_OF_NODES));
  dispose_ll(&linkedlist);
}


Ensure(ll_add, works_also_as_ll_addback)
{
  ll *linkedlist = new_ll(LL_INT);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    ll_addback(linkedlist, &i, NULL);

  assert_that(linkedlist->head->ikey, is_equal_to(0));
  assert_that(linkedlist->len, is_equal_to(NUM_OF_NODES));
  dispose_ll(&linkedlist);
}
