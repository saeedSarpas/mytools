#ifndef AVL_TREE_H
#define AVL_TREE_H


#include <stdlib.h>


typedef struct avl_node
{
  struct avl_node *left, *right;
  int64_t key;
  int height;
  void *data;
} avl_node;


struct avl_node* avl_insert(struct avl_node*, int64_t, void*);
struct avl_node *avl_delete(struct avl_node*, int64_t);
struct avl_node* avl_find(struct avl_node*, int64_t);
void avl_dispose(struct avl_node*);
void inorder_traversal(struct avl_node*, void (*func)(struct avl_node*));
void preorder_traversal(struct avl_node*, void (*func)(struct avl_node*));


#endif /* AVL_TREE_H */
