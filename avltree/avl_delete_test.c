/*
 * avltree/avl_delete_test.c
 * test_for: avltree/avl_delete.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./avl_delete.h"
#include "./avl_insert.h"
#include "./avl_inorder_traversal.h"


#define NUM_OF_NODES 113


static void check_childs(avlnode*, void*);


Describe(avl_delete);
BeforeEach(avl_delete) {}
AfterEach(avl_delete) {}


Ensure(avl_delete, keeps_the_tree_bst)
{
  avltree *tree = new_avltree(set_int_key, compare_float_keys);

  int i;
  for(i = 0; i < NUM_OF_NODES; i += 3)
    avl_insert(tree, &i, NULL);

  i = 5;
  avl_delete(tree, &i);

  avl_inorder_traversal(tree, check_childs, NULL);

  dispose_avltree(tree);
}


static void check_childs(avlnode *node, void *data)
{
  if(node->height > 2){
    assert_true(abs(node->left->height - node->right->height) <= 1);
    assert_true(node->left->key < node->right->key);
  }
}
