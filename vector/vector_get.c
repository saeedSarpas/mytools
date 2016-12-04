/*
 * vector/vector_get.c
 * test_file: vector/vector_get_test.c
 *
 * Retrieving the ith element of the vector
 *
 * Parameters
 * v: The vector
 * i: The index of the element
 *
 * NOTE: Be prepare for the harsh handling of the out of boundary access
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include "./vector_get.h"


void* vector_get(vector *v, int i)
{
  if(i >= (int)v->logLength || i < 0){
    printf("[Error! Trying to get element out of the vector boundaries]\n");
    printf("[       element %d of an array of %d elements]\n", i, v->logLength);
    exit(EXIT_FAILURE);
  }
  return (char*)v->elems + i * v->elemSize;
}
