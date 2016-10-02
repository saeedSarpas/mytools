/*
 * simulation/snapshot.c
 * test_file: simulation/snapshot_test.c
 *
 * creating and destroying generic snapshot structure
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "snapshot.h"
#include "./../memory/allocate.h"


static void dispose_snapshot(snapshot*);


/*
 * creating a new snapshot structure
 *
 * param: tot_num_particles total number of particles
 *
 * return: allocated snapshot structure
 */
snapshot* new_snapshot(int num_darkpart, int num_gaspart, int num_starpart)
{
  snapshot *s = allocate(1, sizeof(snapshot));
  s->header = allocate(1, sizeof(snapshotheader));
  s->darkparts = allocate(num_darkpart, sizeof(snapshotparticle));
  s->gasparts = allocate(num_gaspart, sizeof(snapshotparticle));
  s->starparts = allocate(num_starpart, sizeof(snapshotparticle));
  s->dispose = dispose_snapshot;

  return s;
}


/*
 * disposing a given snapshot structure
 *
 * NOTE: memory leak since we don't free s itself
 */
static void dispose_snapshot(snapshot *s)
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
