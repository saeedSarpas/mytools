/*
 * avltree/avl_delete.c
 * test_file: avltree/avl_delete_test.c
 *
 * delete elements from an AVL tree
 *
 * parameters
 * tree: A generated tree by new_avltree
 * key: The key of the targeted element to be removed
 *
 * author: Saeed Sarpas
 */


#include "./avl_delete.h"


static avlnode* delete(avltree*, avlnode*, void*);
static avlnode* delete_node(avltree*, avlnode*);


void avl_delete(avltree *tree, void *key)
{
  tree->root = delete(tree, tree->root, key);
}


static avlnode* delete(avltree *tree, avlnode *node, void *key)
{
  if(node == NULL) return node;

  if(tree->compare(node->key, key) == AVL_GREATER_THAN)
    node->left = delete(tree, node->left, key);
  else if(tree->compare(node->key, key) == AVL_LESS_THAN)
    node->right = delete(tree, node->right, key);
  else
    node = delete_node(tree, node);

  if(node == NULL) return node;

  node->height = max(height(node->left), height(node->right)) + 1;

  int balance = get_balance(node);

  if(balance > 1 && get_balance(node->left) >= 0)
    return right_rotate(node);

  if(balance > 1 && get_balance(node->left) < 0){
    node->left =  left_rotate(node->left);
    return right_rotate(node);
  }

  if(balance < -1 && get_balance(node->right) <= 0)
    return left_rotate(node);

  if(balance < -1 && get_balance(node->right) > 0){
    node->right = right_rotate(node->right);
    return left_rotate(node);
  }

  return node;
}


static avlnode* delete_node(avltree *tree, avlnode *node)
{
  free(node->key);

  if(node->data != NULL)
    free(node->data);

  if((node->left == NULL) || (node->right == NULL)){
    avlnode *temp = node->left ? node->left : node->right;

    if(temp == NULL){
      temp = node;
      node = NULL;
    } else
      *node = *temp;

    free(temp);
    return node;

  } else {
    avlnode* temp = min_node(node->right);
    node->key = temp->key;
    node->right = delete(tree, node->right, temp->key);
  }

  return node;
}
