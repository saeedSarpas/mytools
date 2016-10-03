#ifndef HALOFINDER_H
#define HALOFINDER_H


#include <inttypes.h>


typedef struct _haloheader {
  int64_t num_halos;
  float Om, Ol, h0, box_size, particle_mass;
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
  struct avl_node *init_volume;
} halo;


typedef struct _halofinder {
  haloheader *header;
  halo *halos;
  void (*dispose)(struct _halofinder*);
} halofinder;


halofinder* new_halofinder(int);
void allocate_particle_ids(halo*, int);


#endif /* HALOFINDER_H */
