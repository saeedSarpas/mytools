/*
 * simulation/siulation_test.c
 * test_for: simulation/siulation.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "simulation.h"


#define NUMPARTS 200


Describe(simulation);
BeforeEach(simulation){};
AfterEach(simulation){};


Ensure(simulation, creates_and_dispose_simulation_struct_correctly)
{
  simulation *hf = new_simulation(NUMPARTS);
  assert_that(hf, is_non_null);
  assert_that(hf->header, is_non_null);
  assert_that(hf->particles, is_non_null);

  hf->dispose(hf);
  assert_that(hf->header, is_null);
  assert_that(hf->particles, is_null);
}
