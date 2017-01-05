/*
 * ll/ll_traverse.c
 * test_file: ll/ll_traverse_test.c
 *
 * Traversing through a given linked list
 *
 * Parameters
 * linkedlist: the linked list
 * func: a desired function to run over each nodes of the linked list
 * data: data provided to the function
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "ll_traverse.h"


void ll_traverse(ll *linkedlist, void (*func)(llnode*, void*), void *data)
{
  llnode *node = linkedlist->head;

  while(node){
    func(node, data);
    node = node->next;
  }
}
