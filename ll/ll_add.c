/*
 * ll/ll_add.c
 * test_file: ll/ll_add_test.c
 *
 * adding a new node to the linked list
 *
 * parameters
 * linkedlist: The linked list
 * key: The key of the new node
 * data: Data to attach to the node (could be NULL too)
 * ... size: Size of the data
 *
 * author: Saeed Sarpas
 */


#include <stdarg.h>
#include <stdlib.h>
#include "./ll_add.h"
#include "./../memory/allocate.h"
#include "./../memory/copy.h"


void ll_add(ll *linkedlist, void *key, llmode mode, void *data, ...)
{
  llnode *newnode = allocate(1, sizeof(*newnode));

  // Copying data if it's available
  if(data != NULL){
    va_list a_list;
    va_start(a_list, data);
    int size = va_arg (a_list, int);
    va_end(a_list);

    newnode->data = allocate(1, size);
    copy(data, newnode->data, size);
  }

  // Copying the key of the node
  copy(key, (char*)&newnode->ikey, linkedlist->sizeofkey);

  if(mode == LL_BACK){
    if(linkedlist->head == NULL){
      newnode->next = NULL;
      linkedlist->head = newnode;
      linkedlist->len += 1;
    }
    else{
    llnode *node = linkedlist->head;

    while(node->next)
      node = node->next;

    node = newnode;
    newnode->next = NULL;
    linkedlist->len += 1;
    }
  }
  else{
    newnode->next = linkedlist->head;
    linkedlist->head = newnode;
    linkedlist->len += 1;
  }
}


void ll_addfront(ll *linkedlist, void *key, void *data, ...)
{
  int size = 0;

  if(data != NULL){
    va_list a_list;
    va_start(a_list, data);
    size = va_arg (a_list, int);
    va_end(a_list);
  }

  ll_add(linkedlist, key, LL_FRONT, data, size);
}


void ll_addback(ll *linkedlist, void *key, void *data, ...)
{
  int size = 0;

  if(data != NULL){
    va_list a_list;
    va_start(a_list, data);
    size = va_arg (a_list, int);
    va_end(a_list);
  }

  ll_add(linkedlist, key, LL_BACK, data, size);
}
