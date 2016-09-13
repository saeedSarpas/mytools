/*
 * simulation/load_gadget_snap.c
 * test_file: simulation/load_gadget_snap_test.c
 *
 * Loading Gadget snapshot
 *
 * param: snapshot Gadget snapshot file
 *
 * NOTE: it only works with single snapshot file and only load ids, positions,
 * velocities, type and masses
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "load_gadget_snap.h"
#include "./../../memory/allocate.h"
#include "./../../io/read_from.h"


struct gadget* load_gadget_snap(FILE *snapshot)
{
  int i, j;
  struct gadget *g = malloc(sizeof *g);

#define SKIPINT fread(&i, sizeof(i), 1, snapshot)

  g->headers = allocate(1, sizeof(struct gadget_header));

  SKIPINT;
  read_from(snapshot, 1, sizeof(struct gadget_header), g->headers);
  SKIPINT;

  int tot_num_particles = 0, ntot_withmasses = 0;
  for(i = 0; i < 6; i++){
    tot_num_particles += g->headers->npart[i];
    if(g->headers->mass[i] == 0.0)
      ntot_withmasses += g->headers->npart[i];
  }

  struct particle *particles = allocate(tot_num_particles,
                                        sizeof(struct particle));

/* Loading positions */
  SKIPINT;
  int offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j++)
      read_from(snapshot, 3, sizeof(float), &particles[j + offset].Pos[0]);
    offset += g->headers->npart[i];
  }
  SKIPINT;

/* Loading velocities */
  SKIPINT;
  offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j++)
      read_from(snapshot, 3, sizeof(float), &particles[j + offset].Vel[0]);
    offset += g->headers->npart[i];
  }
  SKIPINT;

/* Loading ids */
  SKIPINT;
  for(i = 0; i < tot_num_particles; i++)
    read_from(snapshot, 1, sizeof(int), &particles[i].id);
  SKIPINT;

/* Loading masses */
  if(ntot_withmasses > 0) SKIPINT;

  offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j++){
      particles[j + offset].Type = i;

      if(g->headers->mass[i] == 0)
        read_from(snapshot, 1, sizeof(float), &particles[j + offset].Mass);
      else
        particles[j + offset].Mass = g->headers->mass[i];
    }
    offset += g->headers->npart[i];
  }

  if(ntot_withmasses > 0) SKIPINT;

  g->particles = allocate(tot_num_particles, sizeof(struct particle));

  for(i = 0; i < tot_num_particles; i++)
    g->particles[particles[i].id] = particles[i];

  free(particles);

  return g;
}
