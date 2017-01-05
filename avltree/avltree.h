#ifndef AVLTREE_H
#define AVLTREE_H


#include <stdlib.h>


#define AVL_LESS_THAN -1
#define AVL_GREATER_THAN 1
#define AVL_EQUALS 0


typedef struct _avlnode
{
  struct _avlnode *left, *right;
  void *key;
  int height;
  void *data;
} avlnode;


typedef struct _avltree
{
  avlnode *root;
  void (*setkey)(avlnode*, void*);
  int (*compare)(void*, void*);
} avltree;


avltree* new_avltree(void (*setkey)(avlnode*, void*),
                     int (*compare)(void*, void*));
void dispose_avltree(avltree**);
void set_float_key(avlnode*, void*);
void set_int_key(avlnode*, void*);
int compare_float_keys(void*, void*);
int compare_int_keys(void*, void*);
avlnode* right_rotate(avlnode*);
avlnode* left_rotate(avlnode*);
avlnode *min_node(avlnode*);
avlnode *max_node(avlnode*);
int max(int, int);
int height(avlnode*);
int get_balance(avlnode*);


#endif /* AVLTREE_H */
