#ifndef SIMULATION_H
#define SIMULATION_H


#include <inttypes.h>


typedef struct _simulationheader {
  int ndarkpart, ngaspart, nstarspart;
  double mdarkpart, mgaspart, mstarspart;
  double time;
  double redshift;
  double BoxSize;
  double Om, Ol, h0;
} simulationheader;


typedef struct _simulationparticle {
  int64_t id;
  float Pos[3];
  float Vel[3];
  float Mass;
  int Type; // enum ('Gas', 'Halo', 'Disk', 'Bulge', 'Stars', 'Bndry')
  /* float Rho, U, Temp, Ne; */
} simulationparticle;


typedef struct _simulation {
  simulationheader *header;
  simulationparticle *particles;
  void (*dispose)(struct _simulation*);
} simulation;


simulation* new_simulation(int);


#endif /* SIMULATION_H */
