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


/*
 * creating a new snapshot structure
 *
 * param: tot_nparticles total number of particles
 *
 * return: allocated snapshot structure
 */
snapshot* new_snapshot(int tot_nparticles)
{
  snapshot *s = allocate(1, sizeof(snapshot));
  s->header = allocate(1, sizeof(snapshotheader));
  s->header->tot_nparticles = tot_nparticles;
  s->particles = allocate(tot_nparticles, sizeof(snapshotparticle));

  int i;
  for(i = 0; i < s->header->tot_nparticles; i++)
    s->particles[i].id = PARTICLENOTSET;

  return s;
}


/*
 * disposing a given snapshot structure
 *
 * NOTE: memory leak since we don't free s itself
 */
void dispose_snapshot(snapshot *s)
{
  free(s->header);
  s->header = NULL;

  free(s->particles);
  s->particles = NULL;
}
