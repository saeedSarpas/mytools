/*
 * simulation/simulation.c
 * test_file: simulation/simulation_test.c
 *
 * creating and destroying generic simulation structure
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "simulation.h"
#include "./../memory/allocate.h"


static void dispose_simulation(simulation*);


/*
 * creating a new simulation structure
 *
 * param: tot_num_particles total number of particles
 *
 * return: allocated simulation structure
 */
simulation* new_simulation(int num_darkpart, int num_gaspart, int num_starpart)
{
  simulation *s = allocate(1, sizeof(simulation));
  s->header = allocate(1, sizeof(simulationheader));
  s->darkparts = allocate(num_darkpart, sizeof(simulationparticle));
  s->gasparts = allocate(num_gaspart, sizeof(simulationparticle));
  s->starparts = allocate(num_starpart, sizeof(simulationparticle));
  s->dispose = dispose_simulation;

  return s;
}


/*
 * disposing a given simulation structure
 *
 * NOTE: memory leak since we don't free s itself
 */
static void dispose_simulation(simulation *s)
{
  free(s->header);
  s->header = NULL;

  free(s->darkparts);
  s->darkparts = NULL;

  free(s->gasparts);
  s->gasparts = NULL;

  free(s->starparts);
  s->starparts = NULL;
}
