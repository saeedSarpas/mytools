/*
 * vector/vector.c
 * test_file: vector/vector_test.c
 *
 * Allocating and manupulating a vector of a given type and size
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "./../memory/allocate.h"
#include "./../memory/copy.h"
#include "vector.h"


/*
 * Creating a new vector
 *
 * param: alloc_length initial allocation length
 * param: size of each element
 */
struct _vector* vector_new(size_t alloc_length, size_t elem_size)
{
  struct _vector *v = allocate(1, sizeof *v);

  v->elem_size = elem_size;
  v->log_length = 0;
  v->alloc_length = alloc_length;
  v->elems = allocate(alloc_length, elem_size);
  if(v->elems == NULL){
    printf("[Unable to allocate space for the vector]\n");
    exit(EXIT_FAILURE);
  }

  return v;
}


/*
 * Disposing the vector
 *
 * param: v the vector
 */
void vector_dispose(struct _vector *v)
{
  free(v->elems);
}


/*
 * Doubling the size of the array data
 *
 * param: v the vector
 */
static void vector_grow(struct _vector *v)
{
  v->alloc_length *= 2;
  v->elems = realloc(v->elems, v->alloc_length * v->elem_size);
  if(v->elems == NULL){
    printf("[Unable to reallocate the vector]\n");
    exit(EXIT_FAILURE);
  }
}


/*
 * Pushing new element to the vector
 *
 * param: v the vector
 * param: elem_addr pointer to the element to be pushed
 */
void vector_push(struct _vector *v, void *elem_addr)
{
  if(v->log_length == v->alloc_length)
    vector_grow(v);

  void *target = (char*)v->elems + v->log_length * v->elem_size;
  copy(elem_addr, target, v->elem_size);
  v->log_length++;
}


/*
 * Returning the last element of the array and removing it (by decreasing the
 * logical length) from the array
 *
 * param: v the vector
 */
void* vector_pop(struct _vector *v)
{
  v->log_length--;
  return (char*)v->elems + v->log_length * v->elem_size;
}


/*
 * Returning a pointer to the element
 *
 * param: v the vector
 * param: elem_addr pointer to the element to be pushed
 */
void* vector_get_elem(struct _vector *v, int index)
{
  return (char*)v->elems + index * v->elem_size;
}
