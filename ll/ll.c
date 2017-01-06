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
#include "./ll.h"
#include "./ll_traverse.h"
#include "./../memory/allocate.h"


static void freedata(llnode*, void*);
static void delnodes(ll*);


ll* new_ll(int sizeofkey)
{
  ll *linkedlist = allocate(1, sizeof(*linkedlist));
  linkedlist->head = NULL;
  linkedlist->sizeofkey = sizeofkey;
  linkedlist->len = 0;

  return linkedlist;
}


void dispose_ll(ll** linkedlist){
  ll_traverse(*linkedlist, freedata, NULL);

  delnodes((*linkedlist));

  free(*linkedlist);
  *linkedlist = NULL;
}

static void delnodes(ll *list)
{
  while(list->head){
    list->head = list->head->next;
    (list->len)--;
  }
}

static void freedata(llnode *node, void *data)
{
  free(node->data);
  node->data = NULL;
}
