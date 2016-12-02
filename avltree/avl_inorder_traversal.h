#ifndef AVL_INORDER_TRAVERSAL_H
#define AVL_INORDER_TRAVERSAL_H


#include "./avltree.h"


void avl_inorder_traversal(avltree*, void (*func)(avlnode*, void*), void*);


#endif /* AVL_INORDER_TRAVERSAL_H */
