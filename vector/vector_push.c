/*
 * vector/vector_push.c
 * test_file: vector/vector_push_test.c
 *
 * Pushing new element at the end of the vector
 *
 * Parameters
 * v: The vector
 * elem_addr: Pointer to the new element
 *
 * NOTE: Add a new smart shrink function for cases logLength = allocLength / b
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include "./vector_push.h"
#include "./../memory/copy.h"


static void vector_grow(vector *v);


void vector_push(vector *v, void *elem_addr)
{
  if(v->logLength == v->allocLength)
    vector_grow(v);

  void *target = (char*)v->elems + v->logLength * v->elemSize;
  copy(elem_addr, target, v->elemSize);
  v->logLength++;
}


static void vector_grow(struct _vector *v)
{
  v->allocLength *= 2;
  v->elems = realloc(v->elems, v->allocLength * v->elemSize);
  if(v->elems == NULL){
    printf("[Unable to reallocate the vector]\n");
    exit(EXIT_FAILURE);
  }
}
