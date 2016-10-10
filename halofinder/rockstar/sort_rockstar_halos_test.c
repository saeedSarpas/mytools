/*
 * halofinder/sort_rockstar_halos_test.c
 * test_for: halofinder/sort_rockstar_halos.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "load_rockstar_bin.h"
#include "sort_rockstar_halos.h"


Describe(sort_rockstar_halos);


#define ROCKSTAR_FILE_ADDR "./rockstar.bin"
#define INT64 1
#define NUMPARTS 6
#define NUMHALOS 100


BeforeEach(sort_rockstar_halos)
{
  FILE *file;
  file = fopen(ROCKSTAR_FILE_ADDR, "wb");

  int64_t _int64_t = INT64;

  // Filling headers
  rockstarheader rh;
  rh.num_halos = NUMHALOS;
  rh.num_particles = NUMHALOS * NUMPARTS;
  fwrite(&rh, sizeof(rockstarheader), 1, file);

  // Filling halos info
  int i;
  for (i = NUMHALOS; i > 0; i--) {
    int index = NUMHALOS - i;
    int p_start =  index * NUMPARTS;

    rockstarhalo h;
    h.id = i;
    h.m = i * 1e10;
    h.num_p = NUMPARTS;
    h.p_start = p_start;
    fwrite(&h, sizeof(rockstarhalo), 1, file);
  }

  // Filling particle ids
  for(i = 0; i < NUMHALOS; i++)
    fwrite(&_int64_t, sizeof(int64_t), NUMPARTS, file);

  fclose(file);
}


AfterEach(sort_rockstar_halos)
{
  remove(ROCKSTAR_FILE_ADDR);
}


Ensure(sort_rockstar_halos, sorts_halos_properly_based_on_their_mass)
{
  FILE *fp = fopen(ROCKSTAR_FILE_ADDR, "rb");
  halofinder *hf = load_rockstar_bin(fp);
  fclose(fp);

  sort_rockstar_halos(hf->halos, hf->header->num_halos, compare_mass);

  int i;
  for (i = 1; i < NUMHALOS; i++)
    assert_true(hf->halos[i].m >= hf->halos[i-1].m);

  dispose_halofinder(hf);
}


Ensure(sort_rockstar_halos, works_with_real_snapshots)
{
  FILE *fp = fopen("halos_z0_256_100Mpc.bin", "rb");
  halofinder *hf = load_rockstar_bin(fp);
  fclose(fp);

  sort_rockstar_halos(hf->halos, hf->header->num_halos, compare_mass);

  int i;
  for (i = 1; i < hf->header->num_halos; i++)
    assert_true(hf->halos[i].m >= hf->halos[i-1].m);

  dispose_halofinder(hf);
}
