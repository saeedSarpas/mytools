#ifndef SIMULATION_H
#define SIMULATION_H


#include <inttypes.h>


typedef struct _simulationheader {
  int ndarkpart, ngaspart, nstarpart;
  double mdarkpart, mgaspart, mstarpart;
  double time;
  double redshift;
  double boxsize;
  double Om, Ol, h0;
} simulationheader;


typedef struct _simulationparticle {
  int64_t id;
  float Pos[3];
  float Vel[3];
} simulationparticle;


typedef struct _simulation {
  simulationheader *header;
  simulationparticle *darkparts;
  simulationparticle *gasparts;
  simulationparticle *starparts;
  void (*dispose)(struct _simulation*);
} simulation;


simulation* new_simulation(int, int, int);


#endif /* SIMULATION_H */
