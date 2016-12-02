/*
 * avltree/avl_insert.c
 * test_file: avltree/avl_insert_test.c
 *
 * insert elements into an AVL tree
 *
 * parameters
 * tree: A generated tree by new_avltree
 * key: The key of the element
 * data: Data to attach to the avlnode (could be NULL too)
 * ... nmemb: Number of the members of the data
 * ... size: Size of each element of the data
 *
 * author: Saeed Sarpas
 */


#include <stdarg.h>
#include "./avl_insert.h"
#include "./../memory/allocate.h"
#include "./../memory/copy.h"


static avlnode* new_node(avltree*, void*, void*, int, int);
static avlnode* insert(avltree*, avlnode*, void*, void*, int, int);


void avl_insert(avltree *tree, void *key, void *data, ...)
{
  int nmemb = 0, size = 0;

  if(data != NULL){
    va_list a_list;
    va_start(a_list, data);
    nmemb = va_arg (a_list, int);
    size = va_arg (a_list, int);
    va_end(a_list);
  }

  tree->root = insert(tree, tree->root, key, data, nmemb, size);
}


static avlnode* new_node(avltree *tree, void *key, void *data, int nmemb, int size)
{
  tree->root = allocate(1, sizeof(avlnode));

  tree->setkey(tree->root, key);
  tree->root->left = NULL;
  tree->root->right = NULL;
  tree->root->height = 1;

  if(data != NULL){
    tree->root->data = allocate(nmemb, size);
    copy(data, tree->root->data, nmemb * size);
  }

  return tree->root;
}


static avlnode* insert(avltree *tree, avlnode *node, void *key, void *data,
                       int nmemb, int size)
{
  if(node == NULL)
    return new_node(tree, key, data, nmemb, size);

  if(tree->compare(node->key, key) == AVL_GREATER_THAN)
    node->left = insert(tree, node->left, key, data, nmemb, size);
  else if(tree->compare(node->key, key) == AVL_LESS_THAN)
    node->right = insert(tree, node->right, key, data, nmemb, size);
  else
    return node;

  node->height = max(height(node->left), height(node->right)) + 1;

  int balance = get_balance(node);

  if(balance > 1 && key < node->left->key)
    return right_rotate(node);

  if(balance < -1 && key > node->right->key)
    return left_rotate(node);

  if(balance > 1 && key > node->left->key){
    node->left =  left_rotate(node->left);
    return right_rotate(node);
  }

  if(balance < -1 && key < node->right->key){
    node->right = right_rotate(node->right);
    return left_rotate(node);
  }

  return node;
}
