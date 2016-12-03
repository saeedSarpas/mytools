/*
 * vector/vector_sort.c
 * test_file: vector/vector_sort_test.c
 *
 * Sorting the elements of the vector.
 *
 * Parameters
 * v: The vector
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "./vector_sort.h"


void vector_sort(vector *v, int (*compare)(const void*, const void*))
{
  qsort(v->elems, v->logLength, v->elemSize, compare);
}
