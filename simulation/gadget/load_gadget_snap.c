/*
 * simulation/gadget/load_gadget_snap.c
 * test_file: simulation/gadget/load_gadget_snap_test.c
 *
 * Loading Gadget snapshot
 *
 * param: snapshot Gadget snapshot file
 *
 * return: snapshot structure containing header and an array of particles
 *
 * NOTE: it only works with single snapshot file and only load ids, positions,
 * velocities, type and masses
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "load_gadget_snap.h"
#include "gadget_data_type.h"
#include "./../../memory/allocate.h"
#include "./../../io/read_from.h"


static void gadgetheader_to_generic_snapshotheader(gadgetheader*,
                                                   snapshotheader*);
static void set_header_mass(int, float, snapshotheader*);



snapshot* load_gadget_snap(FILE *snapshot)
{
  int i, j;

#define SKIPINT fread(&i, sizeof(i), 1, snapshot)

  gadgetheader* gh = allocate(1, sizeof(gadgetheader));

  SKIPINT;
  read_from(snapshot, 1, sizeof(gadgetheader), gh);
  SKIPINT;

  struct _snapshot *s = new_snapshot(gh->npart[1], gh->npart[0], gh->npart[4]);

  gadgetheader_to_generic_snapshotheader(gh, s->header);

  int tot_num_particles = 0, ntot_withmasses = 0;
  for(i = 0; i < 6; i++){
    tot_num_particles += gh->npart[i];
    if(gh->mass[i] == 0.0)
      ntot_withmasses += gh->npart[i];
  }


// Loading positions
  float dummy_float[3];
  SKIPINT;
  for(i = 0; i < 6; i++){
    switch(i) {
    case 0:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &s->gasparts[j].Pos[0]);
      break;
    case 1:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &s->darkparts[j].Pos[0]);
      break;
    case 2:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &dummy_float);
      break;
    case 3:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &dummy_float);
      break;
    case 4:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &s->starparts[j].Pos[0]);
      break;
    case 5:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &dummy_float);
      break;
    }
  }
  SKIPINT;

// Loading velocities
  SKIPINT;
  for(i = 0; i < 6; i++){
    switch(i) {
    case 0:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &s->gasparts[j].Vel[0]);
      break;
    case 1:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &s->darkparts[j].Vel[0]);
      break;
    case 2:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &dummy_float);
      break;
    case 3:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &dummy_float);
      break;
    case 4:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &s->starparts[j].Vel[0]);
      break;
    case 5:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 3, sizeof(float), &dummy_float);
      break;
    }
  }
  SKIPINT;

// Loading ids
  SKIPINT;
  for(i = 0; i < 6; i++)
    switch(i) {
    case 0:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 1, sizeof(int), &s->gasparts[j].id);
      break;
    case 1:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 1, sizeof(int), &s->darkparts[j].id);
      break;
    case 2:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 1, sizeof(int), &dummy_float);
      break;
    case 3:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 1, sizeof(int), &dummy_float);
      break;
    case 4:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 1, sizeof(int), &s->starparts[j].id);
      break;
    case 5:
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 1, sizeof(int), &dummy_float);
      break;
    }
  SKIPINT;

/* Loading masses */
  if(ntot_withmasses > 0) SKIPINT;

  float tmp_mass;
  for(i = 0; i < 6; i++){
    if(gh->mass[i] == 0){
      for(j = 0; j < gh->npart[i]; j++)
        read_from(snapshot, 1, sizeof(float), &tmp_mass);
      set_header_mass(i, tmp_mass, s->header);
    }
    else
      set_header_mass(i, gh->mass[i], s->header);
  }

  if(ntot_withmasses > 0) SKIPINT;

  return s;
}


/*
 * Converting gadget header to the generic snapshot header
 *
 * param: gh pointer to gadget header to be read
 * param: sh pointer to the allocated simultion header to be filled
 */
static void gadgetheader_to_generic_snapshotheader(gadgetheader *gh,
                                                   snapshotheader *sh)
{
  sh->ndarkpart = gh->npart[1];
  sh->ngaspart = gh->npart[0];
  sh->nstarpart = gh->npart[4];
  sh->mdarkpart = gh->mass[1];
  sh->mgaspart = gh->mass[0];
  sh->mstarpart = gh->mass[4];
  sh->time = gh->time;
  sh->redshift = gh->redshift;
  sh->boxsize = gh->box_size;
  sh->Om = gh->Omega_0;
  sh->Ol = gh->Omega_Lambda;
  sh->h0 = gh->h0;
}


/*
 * Assing mass to different type of particles
 *
 * param: type type of the particle
 * param: mass mass of the particle
 * param: sh pointer to the header member of the snapshot struct
 */
static void set_header_mass(int type, float mass, snapshotheader* sh)
{
  switch(type){
  case 0:
    sh->mgaspart = mass;
    break;
  case 1:
    sh->mdarkpart = mass;
    break;
  case 4:
    sh->mstarpart = mass;
    break;
  }
}
