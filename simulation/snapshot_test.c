/*
 * simulation/snapshot_test.c
 * test_for: simulation/snapshot.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "snapshot.h"


#define NUMPARTS 20


Describe(snapshot);
BeforeEach(snapshot){};
AfterEach(snapshot){};


Ensure(snapshot, creates_and_dispose_snapshot_struct_correctly)
{
  snapshot *hf = new_snapshot(NUMPARTS);
  assert_that(hf, is_non_null);
  assert_that(hf->header, is_non_null);
  assert_that(hf->header->tot_nparticles, is_equal_to(NUMPARTS));
  assert_that(hf->particles, is_non_null);
  int i;
  for(i = 0; i < hf->header->tot_nparticles; i++)
    assert_that(hf->particles[i].id, is_equal_to(PARTICLENOTSET));

  dispose_snapshot(&hf);
  assert_that(hf, is_null);
}
