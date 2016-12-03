#ifndef HALOFINDER_H
#define HALOFINDER_H


#include <inttypes.h>
#include "./../vector/vector.h"


#ifndef HALONOTSET
#define HALONOTSET -1
#endif /* HALONOTSET */


typedef struct _haloheader {
  int64_t num_halos;
  float Om, Ol, h0, box_size[3], particle_mass;
} haloheader;


typedef struct _halo {
  int64_t id;
  float pos[3], vel[3];
  float m, mgrav;
  float r, rvmax, rs;
  float vmax_r, vmax, vrms, halfmass_radius;
  float kin_to_pot;
  float min_pos_err, min_vel_err, min_bulkvel_err;
  int64_t num_p, num_child_particles, p_start, desc, n_core;
  int64_t *particle_ids;
  vector *init_volume; // vector of grid_info s
} halo;


typedef struct _halofinder {
  haloheader *header;
  halo *halos;
} halofinder;


typedef struct _grid_info {
  int index;
  int nParts;
} grid_info;


halofinder* new_halofinder(int);
void allocate_particle_ids(halo*, int);
void dispose_halofinder(halofinder**);


#endif /* HALOFINDER_H */
