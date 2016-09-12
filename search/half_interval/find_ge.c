/**
 * search/half_inteval/find_ge.c
 * test_file search/half_inteval/find_ge_test.c
 *
 * Using half-interval search algorithm to find the first value greater than or
 * equal to the target value in a sorted array
 *
 * param: value target value
 * param: sorted_array sorted array
 * param: array_length Logical length of the array
 * param: compare a function to compare the value of the array at a given index
 * with the the target value. It should return +1 when array_value is greater
 * than the target value, 0 when array_value equals the target value and -1 when
 * array_value is less than target value.
 *
 * author Saeed Sarpas
 */


#include "find_ge.h"


int find_ge(void *value, void *sorted_array, int array_length,
            int (*compare)(const void*, int, const void*))
{
  int min = 0, max = array_length;
  int index = array_length >> 1;

  while (min + 1 < max) {
    if (compare((void*)sorted_array, index, value) == LESS_THAN_T_VALUE)
      min = index + 1;
    else
      max = index;

    index = (max + min) >> 1;
  }

  // In some cases this algorithm will get stuck in the previous element of
  // the expected one
  index = (compare((void*)sorted_array, max - 1, value)) == LESS_THAN_T_VALUE ?
    max : max - 1;

  if (compare((void*)sorted_array, index, value) != EQUAL_TO_T_VALUE)
    return NOT_FOUND;
  else
    return index;
}
