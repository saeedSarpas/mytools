/*
 * simulation/siulation_test.c
 * test_for: simulation/siulation.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "simulation.h"


#define NUMDARKPART 20
#define NUMGASPART 20
#define NUMSTARPART 20


Describe(simulation);
BeforeEach(simulation){};
AfterEach(simulation){};


Ensure(simulation, creates_and_dispose_simulation_struct_correctly)
{
  simulation *hf = new_simulation(NUMDARKPART, NUMGASPART, NUMSTARPART);
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
