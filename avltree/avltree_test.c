/*
 * avltree/avltree_test.c
 * test_for: avltree/avltree.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "avltree.h"


Describe(avltree);
BeforeEach(avltree) {}
AfterEach(avltree) {}

Ensure(avltree, constructor_works_properly)
{
  avltree *tree = new_avltree(set_float_key, compare_float_keys);
  assert_that(tree->root, is_null);
  dispose_avltree(tree);
}
