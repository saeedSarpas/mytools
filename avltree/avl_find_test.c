/*
 * avltree/avl_find_test.c
 * test_for: avltree/avl_find.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "avl_find.h"
#include "avl_insert.h"


#define NUM_OF_NODES 13


Describe(avl_find);
BeforeEach(avl_find) {}
AfterEach(avl_find) {}


Ensure(avl_find, works_properly)
{
  avltree *tree = new_avltree(set_int_key, compare_int_keys);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    avl_insert(tree, &i, NULL);

  avlnode *found = NULL;

  for(i = 0; i < NUM_OF_NODES; i++){
    found = avl_find(tree, &i);
    assert_that(*(int*)found->key, is_equal_to(i));
  }

  i = NUM_OF_NODES;
  found = avl_find(tree, &i);
  assert_that(found, is_null);

  dispose_avltree(&tree);
}


Ensure(avl_find, handles_null_input)
{
  avltree *tree = new_avltree(set_int_key, compare_int_keys);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    avl_insert(tree, &i, NULL);

  avlnode *found = NULL;
  found = avl_find(tree, NULL);

  assert_that(found, is_null);
}
