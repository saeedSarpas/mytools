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
 * return: rockstar structure containing rockstar_headers, an array of halo
 * structures and an array of int64_t particle ids
 *
 * author: Saeed Sarpas
 */


#include "load_rockstar_bin.h"
#include "./../halofinder.h"
#include "./../../memory/allocate.h"
#include "./../../memory/copy.h"
#include "./../../io/open_file.h"
#include "./../../io/read_from.h"


void rockstarheader_to_haloheader(rockstarheader*, haloheader*);
void rockstar_halo_to_generic_halo(rockstarhalo*, halo*);


halofinder* load_rockstar_bin(FILE *file)
{
  rewind(file);

  rockstarheader *rheader = allocate(1, sizeof(*rheader));
  read_from(file, 1, sizeof(*rheader), rheader);

  halofinder *hf = new_halofinder(rheader->num_halos);
  rockstarheader_to_haloheader(rheader, hf->header);

  int i;
  rockstarhalo *rhalo = allocate(1, sizeof(*rhalo));
  for(i = 0; i < hf->header->num_halos; i++){
    read_from(file, 1, sizeof(*rhalo), rhalo);
    rockstar_halo_to_generic_halo(rhalo, &hf->halos[rhalo->id]);
  }

  int64_t *particle_ids = allocate(rheader->num_particles, sizeof(int64_t));
  read_from(file, rheader->num_particles, sizeof(int64_t), particle_ids);

  for(i = 0; i < hf->header->num_halos; i++){
    allocate_particle_ids(&hf->halos[i], hf->halos[i].num_p);
    copy(&particle_ids[hf->halos[i].p_start], hf->halos[i].particle_ids,
         hf->halos[i].num_p * sizeof(int64_t));
  }

  // NOTE memory leak since we didn't free rhalo and rheader pointers
  free(particle_ids);

  return hf;
}


/*
 * Converting rockstar header to the generic header
 *
 * param: rheader pointer to rockstar halo to be read
 * param: header pointer to the halo structure to be filled
 */
void rockstarheader_to_haloheader(rockstarheader *rheader, haloheader *header)
{
  header->num_halos = rheader->num_halos;
  header->Om = rheader->Om;
  header->Ol = rheader->Ol;
  header->h0 = rheader->h0;
  header->box_size = rheader->box_size;
  header->particle_mass = rheader->particle_mass;
}


/*
 * Converting rockstar halo to the generic halo by copying rockstar halo
 * members to the halo struct
 *
 * param: rhalo pointer to rockstar halo to be read
 * param: halo pointer to the halo structure to be filled
 */
void rockstar_halo_to_generic_halo(rockstarhalo *rhalo, halo *halo)
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
  halo->num_child_particles = rhalo->num_child_particles;
  halo->num_p = rhalo->num_p;
  halo->p_start = rhalo->p_start;
  halo->desc = rhalo->desc;
  halo->n_core = rhalo->n_core;
  halo->min_pos_err = rhalo->min_pos_err;
  halo->min_vel_err = rhalo->min_vel_err;
  halo->min_bulkvel_err = rhalo->min_bulkvel_err;
}
