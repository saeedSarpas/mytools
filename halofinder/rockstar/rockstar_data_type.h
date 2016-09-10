#ifndef ROCKSTAR_DATA_TYPE_H
#define ROCKSTAR_DATA_TYPE_H

#include <inttypes.h>
#include "./../shared_halofinder_data_type.h"

#define BINARY_HEADER_SIZE 256
#define VERSION_MAX_SIZE 12


typedef struct rockstar_header {
  uint64_t magic;
  int64_t snap, chunk;
  float scale, Om, Ol, h0;
  float bounds[6];
  int64_t num_halos, num_particles;
  float box_size, particle_mass;
  int64_t particle_type;
  int32_t format_revision;
  char rockstar_version[VERSION_MAX_SIZE];
  char unused[
    BINARY_HEADER_SIZE - (sizeof(char)*VERSION_MAX_SIZE) - (sizeof(float)*12) \
    - sizeof(int32_t) - (sizeof(int64_t)*6)];
} rockstar_header;


typedef struct rockstar_halo {
  int64_t id;
  float pos[6], corevel[3], bulkvel[3];
  float m, r, child_r, vmax_r, mgrav, vmax, rvmax, rs, klypin_rs, vrms, J[3],
    energy, spin, alt_m[4], Xoff, Voff, b_to_a, c_to_a, A[3], b_to_a2, c_to_a2,
    A2[3], bullock_spin, kin_to_pot, m_pe_b, m_pe_d, halfmass_radius;
  int64_t num_p, num_child_particles, p_start, desc, flags, n_core;
  float min_pos_err, min_vel_err, min_bulkvel_err;
} rockstar_halo;

typedef struct rockstar {
  rockstar_header *headers;
  halo *halos;
  int64_t *particle_ids;
} rockstar;


#endif /* ROCKSTAR_DATA_TYPE_H */
