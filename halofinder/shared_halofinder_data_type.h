#ifndef SHARED_HALOFINDER_DATA_TYPE_H
#define SHARED_HALOFINDER_DATA_TYPE_H


#include <inttypes.h>


typedef struct halo {
  int64_t id;
  float pos[3], vel[3];
  float m, mgrav;
  float r, rvmax, rs;
  float vmax_r, vmax, vrms, halfmass_radius;
  float kin_to_pot;
  float min_pos_err, min_vel_err, min_bulkvel_err;
  int64_t num_p, num_child_particles, p_start, desc, n_core;
} halo;


#endif /* SHARED_HALOFINDER_DATA_TYPE_H */
