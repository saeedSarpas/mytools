/*
 * avltree/avtree.c
 * test_file: avltree/avltree_test.c
 *
 * initializing an avltree
 *
 * parameters:
 * setkey: A function to set the keys of the tree
 * compare: A function to compare the key of a given node with a value
 *
 * author: Saeed Sarpas
 */


#include "./avltree.h"
#include "./avl_delete.h"
#include "./../memory/allocate.h"
#include "./../memory/copy.h"


avltree* new_avltree(void (*setkey)(avlnode*, void*), int (*compare)(void*, void*))
{
  avltree *tree = allocate(1, sizeof(*tree));
  tree->root = NULL;
  tree->setkey = setkey;
  tree->compare = compare;

  return tree;
}


void dispose_avltree(avltree **tree)
{
  avlnode *min = NULL;
  while((*tree)->root != NULL){
    min = min_node((*tree)->root);
    avl_delete((*tree), min->key);
  }

  free(*tree);
  *tree = NULL;
}


void set_float_key(avlnode *node, void *value)
{
  node->key = allocate(1, sizeof(float));
  copy(value, node->key, sizeof(float));
}


void set_int_key(avlnode *node, void *value)
{
  node->key = allocate(1, sizeof(int));
  copy(value, node->key, sizeof(int));
}


int compare_float_keys(void *node_key, void *new_key)
{
  if(*(float*)node_key > *(float*)new_key)
    return AVL_GREATER_THAN;
  else if(*(float*)node_key < *(float*)new_key)
    return AVL_LESS_THAN;
  else
    return AVL_EQUALS;
}


int compare_int_keys(void *node_key, void *new_key)
{
  if(*(int*)node_key > *(int*)new_key)
    return AVL_GREATER_THAN;
  else if(*(int*)node_key < *(int*)new_key)
    return AVL_LESS_THAN;
  else
    return AVL_EQUALS;
}


avlnode* right_rotate(avlnode *y)
{
  avlnode *x = y->left;
  avlnode *T2 = x->right;

  x->right = y;
  y->left = T2;

  y->height = max(height(y->left), height(y->right))+1;
  x->height = max(height(x->left), height(x->right))+1;

  return x;
}


avlnode* left_rotate(avlnode *x)
{
  avlnode *y = x->right;
  avlnode *T2 = y->left;

  y->left = x;
  x->right = T2;

  x->height = max(height(x->left), height(x->right))+1;
  y->height = max(height(y->left), height(y->right))+1;

  return y;
}


avlnode *min_node(avlnode *node)
{
  avlnode *current = node;

  while(current->left != NULL)
    current = current->left;

  return current;
}


avlnode *max_node(avlnode *node)
{
  avlnode *current = node;

  while(current->right != NULL)
    current = current->right;

  return current;
}


int max(int a, int b)
{
  return a > b ? a : b;
}


int height(avlnode *node)
{
  return node == NULL ? 0 : node->height;
}


int get_balance(avlnode *node)
{
  return node == NULL ? 0 : height(node->left) - height(node->right);
}
