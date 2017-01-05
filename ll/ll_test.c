/*
 * ll/ll_test.c
 * test_for: ll/ll.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "ll.h"


Describe(ll);
BeforeEach(ll) {}
AfterEach(ll) {}


Ensure(ll, constructor_works_properly)
{
  ll *linkedlist = new_ll(LL_INT);
  assert_that(linkedlist->head, is_null);
  assert_that(linkedlist->sizeofkey, is_equal_to(LL_INT));
  assert_that(linkedlist->len, is_equal_to(0));
  dispose_ll(&linkedlist);
}


Ensure(ll, disposes_the_tree_properly)
{
  ll *linkedlist = new_ll(LL_INT);
  dispose_ll(&linkedlist);
  assert_that(linkedlist, is_null);
}
