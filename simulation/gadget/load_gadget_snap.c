/*
 * simulation/gadget/load_gadget_snap.c
 * test_file: simulation/gadget/load_gadget_snap_test.c
 *
 * Loading Gadget snapshot
 *
 * param: snapshotfile Gadget snapshot file
 *
 * return: snapshot structure containing header and an array of particles
 *
 * NOTE: it only works with single snapshot file and only load ids, positions,
 * velocities, type and masses
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include "load_gadget_snap.h"
#include "gadget_data_type.h"
#include "./../../io/open_file.h"
#include "./../../memory/allocate.h"
#include "./../../io/read_from.h"


static void gadgetheader_to_snapshotheader(gadgetheader*, snapshotheader*);


snapshot* load_gadget_snap(char *snapshotfileaddr)
{
  FILE *snapshotfile = open_file(snapshotfileaddr, "rb");

  int i, j, dummy_int;
  gadgetheader *gh = allocate(1, sizeof(gadgetheader));

#define SKIPINT read_from(snapshotfile, 1, sizeof(dummy_int), &dummy_int)

  SKIPINT;
  read_from(snapshotfile, 1, sizeof(gadgetheader), gh);
  SKIPINT;

  int ntot_particles = 0, ntot_withmasses = 0;
  for(i = 0; i < 6; i++){
    ntot_particles += gh->npart[i];
    if(gh->mass[i] == 0.0)
      ntot_withmasses += gh->npart[i];
  }

  snapshot *s = new_snapshot(ntot_particles);

  gadgetheader_to_snapshotheader(gh, s->header);

// Loading positions
  snapshotparticle *particles = allocate(ntot_particles,
                                         sizeof(snapshotparticle));

  SKIPINT;
  int offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < gh->npart[i]; j++)
      read_from(snapshotfile, 3, sizeof(float), &particles[j + offset].pos[0]);
    offset += gh->npart[i];
  }
  SKIPINT;

// Loading velocities
  SKIPINT;
  offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < gh->npart[i]; j++)
      read_from(snapshotfile, 3, sizeof(float), &particles[j + offset].vel[0]);
    offset += gh->npart[i];
  }
  SKIPINT;

// Loading ids
  SKIPINT;
  for(i = 0; i < ntot_particles; i++)
    read_from(snapshotfile, 1, sizeof(int), &particles[i].id);
  SKIPINT;

// Loading masses
  if(ntot_withmasses > 0) SKIPINT;
  offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < gh->npart[i]; j++){
      particles[j + offset].type = i;

      if(gh->mass[i] == 0)
        read_from(snapshotfile, 1, sizeof(float), &particles[j + offset].mass);
      else
        particles[j + offset].mass = gh->mass[i];
    }
    offset += gh->npart[i];
  }
  if(ntot_withmasses > 0) SKIPINT;

// Sorting particles
  for(i = 0; i < ntot_particles; i++)
    s->particles[particles[i].id] = particles[i];

  free(particles);
  free(gh);

  fclose(snapshotfile);

  return s;
}


/*
 * Converting gadget header to the generic snapshot header
 *
 * param: gh pointer to gadget header to be read
 * param: sh pointer to the allocated simultion header to be filled
 */
static void gadgetheader_to_snapshotheader(gadgetheader *gh, snapshotheader *sh)
{
  int i;
  for(i = 0; i < 6; i++){
    sh->npart[i] = gh->npart[i];
    sh->mass[i] = gh->mass[i];
  }
  sh->time = gh->time;
  sh->redshift = gh->redshift;
  sh->boxsize = gh->box_size;
  sh->Om = gh->Omega_0;
  sh->Ol = gh->Omega_Lambda;
  sh->h0 = gh->h0;
}
