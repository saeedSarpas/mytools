/*
 * simulation/snapshot_test.c
 * test_for: simulation/snapshot.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "snapshot.h"


#define NUMDARKPART 20
#define NUMGASPART 20
#define NUMSTARPART 20


Describe(snapshot);
BeforeEach(snapshot){};
AfterEach(snapshot){};


Ensure(snapshot, creates_and_dispose_snapshot_struct_correctly)
{
  snapshot *hf = new_snapshot(NUMDARKPART, NUMGASPART, NUMSTARPART);
  assert_that(hf, is_non_null);
  assert_that(hf->header, is_non_null);
  assert_that(hf->darkparts, is_non_null);
  assert_that(hf->gasparts, is_non_null);
  assert_that(hf->starparts, is_non_null);

  hf->dispose(hf);
  assert_that(hf->header, is_null);
  assert_that(hf->darkparts, is_null);
  assert_that(hf->gasparts, is_null);
  assert_that(hf->starparts, is_null);
}
