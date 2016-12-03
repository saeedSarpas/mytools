 /*
 * vector/vector.c
 * test_file: vector/vector_test.c
 *
 * Initializing a new vector and disposing it
 *
 * Parameters
 * alloc_length: The length of the vector
 * elem_size: Size of the vector elements
 *
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <stdio.h>
#include "./vector.h"
#include "./../memory/allocate.h"


vector* new_vector(unsigned int alloc_length, unsigned int elem_size)
{
  vector *v = allocate(1, sizeof *v);

  v->elemSize = elem_size;
  v->logLength = 0;
  v->allocLength = alloc_length;
  v->elems = allocate(alloc_length, elem_size);
  if(v->elems == NULL){
    printf("[Unable to allocate space for the vector]\n");
    exit(EXIT_FAILURE);
  }

  return v;
}


void dispose_vector(vector **v)
{
  if((*v)->elems != NULL && (*v)->allocLength != 0){
    free((*v)->elems);
    (*v)->logLength = 0;
    (*v)->elems = NULL;
  }

  free(*v);
  *v = NULL;
}
