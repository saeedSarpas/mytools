/*
 * avltree/avl_inorder_traversal_test.c
 * test_for: avltree/avl_inorder_traversal.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "avl_inorder_traversal.h"
#include "avl_insert.h"


#define NUM_OF_NODES 11


static void increment(avlnode*, void*);
static void check_ascending_order(avlnode*, void*);


Describe(avl_inorder_traversal);
BeforeEach(avl_inorder_traversal) {}
AfterEach(avl_inorder_traversal) {}


Ensure(avl_inorder_traversal, goes_through_all_nodes)
{
  avltree *tree = new_avltree(set_int_key, compare_int_keys);

  int i=0, counter = 0;
  for(i = 0; i < NUM_OF_NODES; i++)
    avl_insert(tree, &i, NULL);

  counter = 0;
  avl_inorder_traversal(tree, increment, &counter);

  assert_that(counter, is_equal_to(NUM_OF_NODES));

  dispose_avltree(tree);
}


Ensure(avl_inorder_traversal, returns_nodes_in_ascending_order)
{
  avltree *tree = new_avltree(set_int_key, compare_int_keys);

  int i;
  for(i = 0; i < NUM_OF_NODES; i++)
    avl_insert(tree, &i, NULL);

  int old_key = -1;
  avl_inorder_traversal(tree, check_ascending_order, &old_key);

  dispose_avltree(tree);
}


static void increment(avlnode *node, void *counter)
{
  *(int*)counter += 1;
}


static void check_ascending_order(avlnode *node, void *old_key)
{
  assert_true(*(int*)node->key > *(int*)old_key);
  old_key = node->key;
}
