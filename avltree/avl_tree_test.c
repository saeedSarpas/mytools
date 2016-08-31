/*
 * avltree/avl_tree_test.c
 * test_for: avltree/avl_tree.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "avl_tree.h"


Describe(avl_tree);
BeforeEach(avl_tree) {}
AfterEach(avl_tree) {}


void check_childs(struct avl_node *node){
  if(node->height > 2){
    assert_true(abs(node->left->height - node->right->height) <= 1);
    assert_true(node->left->key < node->right->key);
  }
}


Ensure(avl_tree, inserts_elements_properly)
{
  struct avl_node *root = NULL;

  // Constructing tree
  int i;
  for(i = 0; i < 100; i++)
    root = avl_insert(root, i);

  inorder_traversal(root, check_childs);
}


Ensure(avl_tree, deletes_elements_properly)
{
  struct avl_node *root = NULL;

  // Constructing tree
  int i;
  for(i = 0; i < 500; i++)
    root = avl_insert(root, i);

  for(i = 0; i < 500; i += 3)
    root = avl_delete(root, i);

  inorder_traversal(root, check_childs);
}
