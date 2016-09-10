/*
 * halofinder/load_rockstar_bin_test.c
 * test_for: halofinder/load_rockstar_bin.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <math.h>
#include "load_rockstar_bin.h"


Describe(load_rockstar_bin);


#define ROCKSTAR_FILE_ADDR "./rockstar.bin"
#define NUMPARTS 6
#define NUMHALOS 33
#define HALOPOS 1.234
#define HALOVEL 5.678
#define HALOMASS 123456789000.0
#define HALORADIUS 1234.0
#define PARTMASS 4.56e9
#define MASSPRECISION 1E5
#define RADIUSPRECISION 1E0
#define UNUSEDBYTES BINARY_HEADER_SIZE - (sizeof(char)*VERSION_MAX_SIZE) \
  - (sizeof(float)*12) - sizeof(int32_t) - (sizeof(int64_t)*6)


BeforeEach(load_rockstar_bin)
{
  FILE *file;
  file = fopen(ROCKSTAR_FILE_ADDR, "wb");

  // Filling headers
  struct rockstar_header rh;
  rh.num_halos = NUMHALOS;
  rh.num_particles = NUMHALOS * NUMPARTS;
  fwrite(&rh, sizeof(struct rockstar_header), 1, file);

  // Filling halos info
  int64_t i, j, p_start;

  for(i = 0; i < NUMHALOS; i++) {
    p_start =  i * NUMPARTS;

    struct rockstar_halo h;
    h.id = i;
    for(j = 0; j < 3; j++)
      h.pos[j] = HALOPOS;
    for(j = 3; j < 6; j++)
      h.pos[j] = HALOVEL;
    h.m = HALOMASS;
    h.r = HALORADIUS;
    h.num_p = NUMPARTS;
    h.p_start = p_start;
    fwrite(&h, sizeof(struct rockstar_halo), 1, file);
  }

  // Filling particle ids
  for(i = 0; i < NUMHALOS; i++)
    for(j = 0; j < NUMPARTS; j++)
      fwrite(&i, 1, sizeof(int64_t), file);

  fclose(file);
}


AfterEach(load_rockstar_bin)
{
  remove(ROCKSTAR_FILE_ADDR);
}


Ensure(load_rockstar_bin, loads_halo_data_properly)
{
  FILE *fp = fopen(ROCKSTAR_FILE_ADDR, "rb");
  struct rockstar *r = load_rockstar_bin(fp);
  fclose(fp);

  // Testing header
  assert_that(r->headers->num_halos, is_equal_to(NUMHALOS));
  assert_that(r->headers->num_particles, is_equal_to(NUMHALOS * NUMPARTS));

  // Testing halos
  int i, j;
  for(i = 0; i < NUMHALOS; i++){
    assert_that(r->halos[i].id, is_equal_to(i));
    for(j = 0; j < 3; j++){
      assert_that(r->halos[i].pos[j], is_equal_to(HALOPOS));
      assert_that(r->halos[i].vel[j], is_equal_to(HALOVEL));
    }
    assert_true(fabs(r->halos[i].m - HALOMASS) <= MASSPRECISION);
    assert_true(fabs(r->halos[i].r - HALORADIUS) <= RADIUSPRECISION);
    assert_that(r->halos[i].num_p, is_equal_to(NUMPARTS));
  }

  // Testing particle ids
  for(i = 0; i < NUMHALOS; i++)
    for(j = 0; j < NUMPARTS; j++)
      assert_that(r->particle_ids[i * NUMPARTS + j], is_equal_to(i));

  free(r);
}
