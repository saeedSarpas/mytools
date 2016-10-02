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
simulation* new_simulation(int tot_num_particles)
{
  simulation *s = allocate(1, sizeof(simulation));
  s->header = allocate(1, sizeof(simulationheader));
  s->particles = allocate(tot_num_particles, sizeof(simulationparticle));
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

  free(s->particles);
  s->particles = NULL;
}
