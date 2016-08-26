/*
 * simulation/load_gadget_snap.c
 * test_file: simulation/load_gadget_snap_test.c
 *
 * Loading Gadget snapshot
 *
 * param: file Gadget snapshot file
 *
 * NOTE: it only works with single snapshot file and only load ids, positions
 * and velocities
 *
 * author: Saeed Sarpas
 */


#include "load_gadget_snap.h"


struct gadget* load_gadget_snap(FILE *file){
  int i, j;
  struct gadget *g = malloc(sizeof *g);

#define SKIPINT fread(&i, sizeof(i), 1, file)

  g->headers = allocate(1, sizeof(struct gadget_header));

  SKIPINT;
  read_from(file, 1, sizeof(struct gadget_header), g->headers);
  SKIPINT;

  int tot_num_particles= 0;
  for(i = 0; i < 6; i++)
    tot_num_particles += g->headers->npart[i];

  g->particles = allocate(tot_num_particles, sizeof(struct gadget_particle));

  /* Loading positions */
  SKIPINT;
  int offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j++)
      read_from(file, 3, sizeof(float), &g->particles[j + offset].Pos[0]);
    offset += g->headers->npart[i];
  }
  SKIPINT;

  /* Loading velocities */
  SKIPINT;
  offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j++)
      read_from(file, 3, sizeof(float), &g->particles[j + offset].Vel[0]);
    offset += g->headers->npart[i];
  }
  SKIPINT;

  /* Loading ids */
  SKIPINT;
  for(i = 0; i < tot_num_particles; i++)
    read_from(file, 1, sizeof(int), &g->particles[i].id);
  SKIPINT;

  return g;
}
