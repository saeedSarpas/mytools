/**
 * search/half_inteval/find_ge_test.c
 * test_for search/half_inteval/find_ge.c
 *
 * author Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "find_ge.h"


#define ARRAYLENGTH 111


Describe(find_ge);
BeforeEach(find_ge){}
AfterEach(find_ge){}


int compare_int(const void*, int, const void*);
int compare_double(const void*, int, const void*);


Ensure(find_ge, finds_the_first_element_for_a_constant_array)
{
  int i, array[ARRAYLENGTH];
  for(i = 0; i < ARRAYLENGTH; i++)
    array[i] = 0;

  int target_value = 0;
  int index = find_ge((void*)&target_value, array, ARRAYLENGTH, compare_int);
  assert_that(index, is_equal_to(0));

  target_value = 1;
  index = find_ge((void*)&target_value, array, ARRAYLENGTH, compare_int);
  assert_that(index, is_equal_to(NOT_FOUND));
}


Ensure(find_ge, finds_right_element_in_an_ordered_array)
{
  int i, index, array[ARRAYLENGTH];
  for(i = 0; i < ARRAYLENGTH; i++)
    array[i] = i / 3;

  for(i = 0; i < ARRAYLENGTH / 3; i++){
    index = find_ge((void*)&i, array, ARRAYLENGTH, compare_int);
    assert_that(index, is_equal_to(3 * i));
  }
}


Ensure(find_ge, finds_right_element_even_if_it_rests_at_the_end_of_the_array)
{
  int i, target_value, array[ARRAYLENGTH];
  for(i = 0; i < ARRAYLENGTH; i++)
    array[i] = 0;

  target_value = 1;

  array[ARRAYLENGTH - 1] = 1;
  int index = find_ge((void*)&target_value, array, ARRAYLENGTH, compare_int);
  assert_that(index, is_equal_to(ARRAYLENGTH - 1));

  array[ARRAYLENGTH - 2] = 1;
  index = find_ge((void*)&target_value, array, ARRAYLENGTH, compare_int);
  assert_that(index, is_equal_to(ARRAYLENGTH - 2));
}


Ensure(find_ge, finds_right_element_in_an_ordered_array_of_doubles)
{
  int i, index;
  double wanted_value, array[ARRAYLENGTH];
  for(i = 0; i < ARRAYLENGTH; i++)
    array[i] = 1 + .1 * i;

  wanted_value = 1.0;
  index = find_ge((void*)&wanted_value, array, ARRAYLENGTH, compare_double);
  assert_that(index, is_equal_to(0));

  wanted_value = 1.05;
  index = find_ge((void*)&wanted_value, array, ARRAYLENGTH, compare_double);
  assert_that(index, is_equal_to(1));

  wanted_value = 1.1;
  index = find_ge((void*)&wanted_value, array, ARRAYLENGTH, compare_double);
  assert_that(index, is_equal_to(1));
}


int compare_int(const void *sorted_array, int index, const void *target_value)
{
  const int a_value = ((int*)sorted_array)[index];
  const int t_value = *(int*)target_value;

  if(a_value < t_value)
    return LESS_THAN_T_VALUE;
  else if(a_value > t_value)
    return GREATER_THAN_T_VALUE;
  else
    return EQUAL_TO_T_VALUE;
}


int compare_double(const void *sorted_array, int index, const void *target_value)
{
  const double a_value = ((double*)sorted_array)[index];
  const double t_value = *(double*)target_value;

  if(a_value < t_value)
    return LESS_THAN_T_VALUE;
  else if(a_value > t_value)
    return GREATER_THAN_T_VALUE;
  else
    return EQUAL_TO_T_VALUE;
}
