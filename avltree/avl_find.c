/*
 * avltree/avl_find.c
 * test_file: avltree/avl_find_test.c
 *
 * retrieving element from an AVL tree
 *
 * parameters
 * tree: A generated tree by new_avltree
 * key: The key of the targeted element to be removed
 *
 * author: Saeed Sarpas
 */


#include "./avl_find.h"


static avlnode* find(avltree*, avlnode*, void*);


avlnode* avl_find(avltree *tree, void *key)
{
  if(key == NULL) return NULL;

  return find(tree, tree->root, key);
}


static avlnode* find(avltree *tree, avlnode *node, void *key)
{
  if(node == NULL) return node;

  if(tree->compare(node->key, key) == AVL_GREATER_THAN)
    node = find(tree, node->left, key);
  else if(tree->compare(node->key, key) == AVL_LESS_THAN)
    node = find(tree, node->right, key);

  return node;
}
