#ifndef SHARED_SIMULATION_DATA_TYPE_H
#define SHARED_SIMULATION_DATA_TYPE_H


typedef struct particle
{
  int id;
  float Pos[3];
  float Vel[3];
  /* float Mass; */
  /* int Type; */
  /* float Rho, U, Temp, Ne; */
} particle;


#endif /* SHARED_SIMULATION_DATA_TYPE_H */
