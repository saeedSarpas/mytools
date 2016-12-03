/*
 * vector/vector_pop.c
 * test_file: vector/vector_pop_test.c
 *
 * Poping the elements of the vector (Retrieving the last element)
 *
 * Parameters
 * v: The vector
 *
 * NOTE: Be prepare for the harsh handling of the empty vectors
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include "./vector_pop.h"


void* vector_pop(vector *v)
{
  if(v->logLength > 0){
    v->logLength--;
    return (char*)v->elems + v->logLength * v->elemSize;
  } else {
    printf("[Vector is empty!]\n");
    exit(EXIT_FAILURE);
  }
}
