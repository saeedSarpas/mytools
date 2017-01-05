/*
 * ll/ll.c
 * test_file: ll/ll_test.c
 *
 * Construction a new linked list
 *
 * Parameters
 * sizeofkey: linked list key type (e.g. LL_INT, LL_FLOAT)
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "ll.h"
#include "./../memory/allocate.h"


ll* new_ll(int sizeofkey)
{
  ll *linkedlist = allocate(1, sizeof(*linkedlist));
  linkedlist->head = NULL;
  linkedlist->sizeofkey = sizeofkey;
  linkedlist->len = 0;

  return linkedlist;
}


void dispose_ll(ll** linkedlist){
  free(*linkedlist);
  *linkedlist = NULL;
}
