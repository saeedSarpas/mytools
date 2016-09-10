/*
 * halofinder/load_rockstar_bin.c
 * test_file: halofinder/load_rockstar_bin_test.c
 *
 * Loading rockstar binary output file
 *
 * param: file rockstar file
 * param: rockstar a struct containing pointers to rockstar headers, rockstar
 * halos and rockstar particle ids.
 *
 * author: Saeed Sarpas
 */


#include "load_rockstar_bin.h"
#include "./../../memory/allocate.h"
#include "./../../io/open_file.h"
#include "./../../io/read_from.h"


void rockstar_halo_to_generic_halo(struct rockstar_halo*, struct halo*);


struct rockstar* load_rockstar_bin(FILE *file)
{
  struct rockstar *r = malloc(sizeof *r);

  r->headers = allocate(1, sizeof(struct rockstar_header));
  read_from(file, 1, sizeof(struct rockstar_header), r->headers);

  r->halos = allocate(r->headers->num_halos, sizeof(struct halo));
  r->particle_ids = allocate(r->headers->num_particles, sizeof(int64_t));

  struct rockstar_halo *rhalo = allocate(1, sizeof(struct halo));

  int i;
  for(i = 0; i < r->headers->num_halos; i++){
    read_from(file, 1, sizeof(struct rockstar_halo), rhalo);
    rockstar_halo_to_generic_halo(rhalo, &r->halos[i]);
  }

  read_from(file, r->headers->num_particles, sizeof(int64_t), r->particle_ids);

  free(rhalo);
  return r;
}


/*
 * Converting rockstar halo to the generic halo by copying rockstar halo
 * members to the halo struct
 *
 * param: rhalo pointer to rockstar halo to be read
 * param: halo pointer to the halo structure to be filled
 */
void rockstar_halo_to_generic_halo(struct rockstar_halo *rhalo,
                                   struct halo *halo)
{
  int i;
  halo->id = rhalo->id;
  for(i = 0; i < 3; i++)
    halo->pos[i] = rhalo->pos[i];
  for(i = 3; i < 6; i++)
    halo->vel[i - 3] = rhalo->pos[i];
  halo->m = rhalo->m;
  halo->r = rhalo->r;
  halo->vmax_r = rhalo->vmax_r;
  halo->mgrav = rhalo->mgrav;
  halo->vmax = rhalo->vmax;
  halo->rvmax = rhalo->rvmax;
  halo->rs = rhalo->rs;
  halo->vrms = rhalo->vrms;
  halo->kin_to_pot = rhalo->kin_to_pot;
  halo->halfmass_radius = rhalo->halfmass_radius;
  halo->num_p = rhalo->num_p;
  halo->num_child_particles = rhalo->num_child_particles;
  halo->p_start = rhalo->p_start;
  halo->desc = rhalo->desc;
  halo->n_core = rhalo->n_core;
  halo->min_pos_err = rhalo->min_pos_err;
  halo->min_vel_err = rhalo->min_vel_err;
  halo->min_bulkvel_err = rhalo->min_bulkvel_err;
}
