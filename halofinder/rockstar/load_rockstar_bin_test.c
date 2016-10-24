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
#define PARTID 123
#define MASSPRECISION 1E5
#define RADIUSPRECISION 1E0
#define UNUSEDBYTES BINARY_HEADER_SIZE - (sizeof(char)*VERSION_MAX_SIZE) \
  - (sizeof(float)*12) - sizeof(int32_t) - (sizeof(int64_t)*6)


BeforeEach(load_rockstar_bin)
{
  FILE *file;
  file = fopen(ROCKSTAR_FILE_ADDR, "wb");

  // Filling headers
  rockstarheader rh;
  rh.num_halos = NUMHALOS;
  rh.num_particles = NUMHALOS * NUMPARTS;
  fwrite(&rh, sizeof(rockstarheader), 1, file);

  // Filling halos info
  int64_t i, j, p_start;

  for(i = 0; i < NUMHALOS; i++) {
    p_start =  i * NUMPARTS;

    rockstarhalo h;
    h.id = i;
    for(j = 0; j < 3; j++)
      h.pos[j] = HALOPOS;
    for(j = 3; j < 6; j++)
      h.pos[j] = HALOVEL;
    h.m = HALOMASS;
    h.r = HALORADIUS;
    h.num_p = NUMPARTS;
    h.p_start = p_start;
    fwrite(&h, sizeof(rockstarhalo), 1, file);
  }

  // Filling particle ids
  int64_t partid = PARTID;
  for(i = 0; i < NUMHALOS; i++)
    for(j = 0; j < NUMPARTS; j++)
      fwrite(&partid, 1, sizeof(int64_t), file);

  fclose(file);
}


AfterEach(load_rockstar_bin)
{
  remove(ROCKSTAR_FILE_ADDR);
}


Ensure(load_rockstar_bin, loads_halo_data_properly)
{
  halofinder *hf = load_rockstar_bin(ROCKSTAR_FILE_ADDR);

  // Testing header
  assert_that(hf->header->num_halos, is_equal_to(NUMHALOS));

  // Testing halos
  int i, j;
  for(i = 0; i < NUMHALOS; i++){
    assert_that(hf->halos[i].id, is_equal_to(i));
    for(j = 0; j < 3; j++){
      assert_that(hf->halos[i].pos[j], is_equal_to(HALOPOS));
      assert_that(hf->halos[i].vel[j], is_equal_to(HALOVEL));
    }
    assert_true(fabs(hf->halos[i].m - HALOMASS) <= MASSPRECISION);
    assert_true(fabs(hf->halos[i].r - HALORADIUS) <= RADIUSPRECISION);
    assert_that(hf->halos[i].num_p, is_equal_to(NUMPARTS));

    // Testing particle ids
    for(j = 0; j < NUMPARTS; j++)
      assert_that(hf->halos[i].particle_ids[j], is_equal_to(PARTID));
  }

  dispose_halofinder(hf);
}


Ensure(load_rockstar_bin, works_with_real_rockstar_binary_file)
{
  halofinder *hf = load_rockstar_bin("./halos_z0_256_100Mpc.bin");

  // Testing header
  assert_that_double(hf->header->Om, is_equal_to_double(0.308900));
  assert_that_double(hf->header->Ol, is_equal_to_double(0.691100));
  assert_that_double(hf->header->h0, is_equal_to_double(0.677400));
  assert_that(hf->header->box_size[0], is_equal_to(100));
  assert_that(hf->header->box_size[1], is_equal_to(100));
  assert_that(hf->header->box_size[2], is_equal_to(100));
  assert_that(hf->header->particle_mass, is_equal_to(5109658624));

  // Testing halos
  int i, j;
  for(i = 0; i < hf->header->num_halos; i++){
    assert_that(hf->halos[i].id, is_equal_to(i));
    for(j = 0; j < 3; j++){
      assert_true(0 < hf->halos[i].pos[j]
                  && hf->halos[i].pos[j] < hf->header->box_size[j]);
    }

    // Testing particle ids
    for(j = 0; j < hf->halos[i].num_p; j += 1e3){
      assert_true(0 <= hf->halos[i].particle_ids[j]
                  && hf->halos[i].particle_ids[j] < pow(256, 3));
    }
  }

  dispose_halofinder(hf);
}
