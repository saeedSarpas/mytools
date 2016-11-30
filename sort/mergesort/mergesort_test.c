/**
 * sort/mergesort/mergesort_test.c
 * test_for sort/mergesort/mergesort.c
 *
 * author Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./mergesort.h"


#define ARRAYLENGTH 111


Describe(mergesort);
BeforeEach(mergesort){}
AfterEach(mergesort){}


static int compare_double(void*, int, void*, int);
static void set_double(void*, int, void*, int);


Ensure(mergesort, finds_the_first_element_for_a_constant_array)
{
  double arr[9] = {1.23, 3.45, 2.34, 5.67, 4.56, 7.89, 6.78, 9.01, 8.90};
  mergesort(arr, 0, 8, set_double, compare_double, sizeof(double));

  int i;
  for(i = 1; i < 9; i++)
    assert_that(arr[i] > arr[i-1]);
}


static int compare_double(void *arr1, int index1, void *arr2, int index2)
{
  const double v1 = ((double*)arr1)[index1];
  const double v2 = ((double*)arr2)[index2];

  if(v1 < v2)
    return MS_LESS_THAN;
  else if(v1 > v2)
    return MS_GREATER_THAN;
  else
    return MS_EQUALS;
}

static void set_double(void *target_arr, int target_index, void *arr, int index)
{
  ((double*)target_arr)[target_index] = ((double*)arr)[index];
}
