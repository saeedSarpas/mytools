/*
 * ll/ll_pop.c
 * test_file: ll/ll_pop_test.c
 *
 * popping the first item of the list
 *
 * parameters
 * linkedlist: The linked list
 *
 * return
 * the popped node
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "./ll_pop.h"


llnode* ll_pop(ll *linkedlist)
{
  if(linkedlist->head == NULL)
    return NULL;

  llnode *oldhead = linkedlist->head;
  linkedlist->head = oldhead->next;
  (linkedlist->len)--;

  return oldhead;
}
