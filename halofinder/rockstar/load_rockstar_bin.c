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


struct rockstar* load_rockstar_bin(FILE *file){
  struct rockstar *r = malloc(sizeof *r);

  r->headers = allocate(1, sizeof(struct rockstar_header));
  read_from(file, 1, sizeof(struct rockstar_header), r->headers);

  r->halos = allocate(r->headers->num_halos, sizeof(struct rockstar_halo));
  r->particles = allocate(r->headers->num_particles, sizeof(int64_t));

  read_from(file, r->headers->num_halos, sizeof(struct rockstar_halo), r->halos);
  read_from(file, r->headers->num_particles, sizeof(int64_t), r->particles);

  return r;
}
