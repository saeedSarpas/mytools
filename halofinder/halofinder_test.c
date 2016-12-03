/*
 * halofinder/halofinder_test.c
 * test_for: halofinder/halofinder.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "halofinder.h"
#include "./../memory/allocate.h"


#define NUMHALOS 11
#define NUMPARTS 2


Describe(halofinder);
BeforeEach(halofinder){};
AfterEach(halofinder){};


Ensure(halofinder, creates_and_dispose_halofinder_struct_correctly)
{
  halofinder *hf = new_halofinder(NUMHALOS);
  assert_that(hf, is_non_null);
  assert_that(hf->header, is_non_null);
  assert_that(hf->header->num_halos, is_equal_to(NUMHALOS));
  assert_that(hf->halos, is_non_null);

  assert_that(hf->halos[0].particle_ids, is_null);
  allocate_particle_ids(&hf->halos[0], NUMPARTS);
  assert_that(hf->halos[0].particle_ids, is_non_null);

  dispose_halofinder(&hf);
}
