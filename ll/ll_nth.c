
/*
 * ll/ll_nth.c
 * test_file: ll/ll_nth_test.c
 *
 * Returning the nth element of the linked list
 *
 * Parameters
 * linkedlist: the linked list
 * n: the index of the element
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "ll_nth.h"


llnode* ll_nth(ll *linkedlist, int n)
{
  if(n > linkedlist->len - 1 || n < -1 * linkedlist->len)
    return NULL;

  if(n < 0)
    n += linkedlist->len;

  llnode *node = linkedlist->head;

  int i;
  for(i = 0; i < n; i++){
    node = node->next;

  }

  return node;
}
