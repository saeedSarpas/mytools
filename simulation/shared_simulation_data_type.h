#ifndef SHARED_SIMULATION_DATA_TYPE_H
#define SHARED_SIMULATION_DATA_TYPE_H


#include <inttypes.h>


typedef struct particle
{
  int64_t id;
  float Pos[3];
  float Vel[3];
  float Mass;
  int Type; // enum ('Gas', 'Halo', 'Disk', 'Bulge', 'Stars', 'Bndry')
  /* float Rho, U, Temp, Ne; */
} particle;


#endif /* SHARED_SIMULATION_DATA_TYPE_H */
