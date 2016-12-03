/*
 * avltree/avl_insert_test.c
 * test_for: avltree/avl_insert.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "avl_insert.h"
#include "avl_inorder_traversal.h"


Describe(avl_insert);
BeforeEach(avl_insert) {}
AfterEach(avl_insert) {}


#define NUM_OF_NODES 17


static void check_childs(avlnode*, void*);
static void check_int_data(avlnode*, void*);
static void increment(avlnode*, void*);


Ensure(avl_insert, is_able_to_create_a_new_tree)
{
  avltree* tree = new_avltree(set_float_key, compare_float_keys);

  float id = 1.23;
  avl_insert(tree, &id,  NULL);
  assert_that(tree->root, is_non_null);

  dispose_avltree(&tree);
}


Ensure(avl_insert, keeps_the_tree_bst)
{
  avltree *tree = new_avltree(set_int_key, compare_int_keys);

  int i;
  for(i=0; i < NUM_OF_NODES; i++)
    avl_insert(tree, &i, NULL);

  avl_inorder_traversal(tree, check_childs, NULL);

  dispose_avltree(&tree);
}


Ensure(avl_insert, prevents_from_adding_nodes_with_the_same_keys)
{
  avltree *tree = new_avltree(set_int_key, compare_int_keys);

  int i, identical_key = 1;
  for(i = 0; i < NUM_OF_NODES; i++)
    avl_insert(tree, &identical_key, NULL);

  int counter = 0;
  avl_inorder_traversal(tree, increment, &counter);

  assert_that(counter, is_equal_to(1));

  dispose_avltree(&tree);
}


Ensure(avl_insert, set_the_data_properly)
{
  avltree *tree = new_avltree(set_int_key, compare_int_keys);

  int i;
  for(i = 0; i < 10; i++)
    avl_insert(tree, &i, &i, 1, sizeof(int));

  avl_inorder_traversal(tree, check_int_data, NULL);

  dispose_avltree(&tree);
}


static void check_childs(avlnode *node, void *data)
{
  if(node->height > 2){
    assert_true(abs(node->left->height - node->right->height) <= 1);
    assert_true(node->left->key < node->right->key);
  }
}


static void check_int_data(avlnode *node, void *data)
{
  assert_that(*(int*)node->data, is_equal_to(*(int*)node->key));
}


static void increment(avlnode *node, void *counter)
{
  *(int*)counter += 1;
}
