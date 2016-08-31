#ifndef AVL_TREE_H
#define AVL_TREE_H


#include <stdlib.h>


typedef struct avl_node
{
  struct avl_node *left, *right;
  int key;
  int height;
  void *data;
} avl_node;


struct avl_node* avl_insert(struct avl_node*, int);
struct avl_node *avl_delete(struct avl_node*, int);
void inorder_traversal(struct avl_node*, void (*func)(struct avl_node*));


#endif /* AVL_TREE_H */
