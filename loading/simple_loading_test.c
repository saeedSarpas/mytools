/*
 * loading/simple_loading_test.c
 * test_for: loading/simple_loading_tree.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "simple_loading.h"


Describe(simple_loading);
BeforeEach(simple_loading) {};
AfterEach(simple_loading) {};


Ensure(simple_loading, returns_right_amount_for_a_given_parameters)
{
  int i, percent;
  for(i = 0; i <= 100; i++){
    percent = simple_loading(i, i, 100);
    assert_that(percent, is_equal_to(i));
  }
}
