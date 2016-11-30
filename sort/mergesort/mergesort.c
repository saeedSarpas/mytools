/**
 * sort/mergesort/mergesort.c
 * test_file sort/mergesort/mergesort_test.c
 *
 * Sorting array using merge sort algorithm
 *
 * param: arr the array to be sorted
 * param: lindex index of the most left element of the array
 * param: rindex index of the most right element of the array
 * param: compare a function to compare two elements of the array
 *
 * author Saeed Sarpas
 */


#include <stdlib.h>
#include "./mergesort.h"
#include "./../../memory/allocate.h"
#include "./../../memory/copy.h"


static void merge(void*, int, int, int, void (*set)(void*, int, void*, int),
                  int (*compare)(void*, int, void*, int), unsigned int);


void mergesort(void *arr, int lindex, int rindex,
               void (*set)(void*, int, void*, int),
               int (*compare)(void*, int, void*, int), unsigned int elem_size)
{
  if(lindex < rindex){
    int m = (lindex + rindex)/2;

    mergesort(arr, lindex, m, set, compare, elem_size);
    mergesort(arr, m+1, rindex, set, compare, elem_size);

    merge(arr, lindex, m, rindex, set, compare, elem_size);
  }
  return;
}


static void merge(void *arr, int lindex, int m, int rindex,
                  void (*set)(void*, int, void*, int),
                  int (*compare)(void*, int, void*, int), unsigned int elem_size)
{
  int i, j, k;
  int n1 = m - lindex + 1;
  int n2 =  rindex - m;

  void *L = allocate(n1, elem_size);
  void *R = allocate(n1, elem_size);

  copy((char*)arr + lindex * elem_size, L, n1 * elem_size);
  copy((char*)arr + (m + 1) * elem_size, R, n2 * elem_size);

  i = 0; j = 0; k = lindex;

  while(i < n1 && j < n2){
    if(compare(L, i, R, j) != MS_GREATER_THAN){
      set(arr, k, L, i);
      i++;
    } else {
      set(arr, k, R, j);
      j++;
    }
    k++;
  }

  while(i < n1){
    set(arr, k, L, i);
    i++;
    k++;
  }

  while(j < n2){
    set(arr, k, R, j);
    j++;
    k++;
  }

  free(L);
  free(R);
}
