/*
 * halofinder/load_rockstar_bin_test.c
 * test_for: halofinder/load_rockstar_bin.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "load_rockstar_bin.h"


Describe(load_rockstar_bin);


#define ROCKSTAR_FILE_ADDR "./rockstar.bin"
#define UINT64 0
#define INT64 1
#define INT32 2
#define FLOAT 3.4
#define EMPTYSTR ""
#define NUMPARTS 6
#define NUMHALOS 3
#define UNUSEDBYTES BINARY_HEADER_SIZE - (sizeof(char)*VERSION_MAX_SIZE) \
    - (sizeof(float)*12) - sizeof(int32_t) - (sizeof(int64_t)*6)


BeforeEach(load_rockstar_bin)
{
  FILE *file;
  file = fopen(ROCKSTAR_FILE_ADDR, "wb");

  int64_t _int64_t = INT64;

  /* Filling headers */
  struct rockstar_header rh;
  rh.magic = UINT64;
  rh.num_halos = NUMHALOS;
  rh.num_particles = NUMHALOS * NUMPARTS;
  fwrite(&rh, sizeof(struct rockstar_header), 1, file);

  /* Filling halos info */
  int i;
  for (i = 0; i < NUMHALOS; i++) {
    int p_start =  i * NUMPARTS;

    struct rockstar_halo h;
    h.id = i;
    h.num_p = NUMPARTS;
    h.p_start = p_start;
    fwrite(&h, sizeof(struct rockstar_halo), 1, file);
  }

  /* Filling particle ids */
  for (i = 0; i < NUMHALOS; i++)
    fwrite(&_int64_t, sizeof(int64_t), NUMPARTS, file);

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

  assert_that(r->headers->magic, is_equal_to(UINT64));
  assert_that(r->headers->num_halos, is_equal_to(NUMHALOS));
  assert_that(r->headers->num_particles, is_equal_to(NUMHALOS * NUMPARTS));

  assert_that(r->halos[0].id, is_equal_to(0));
  assert_that(r->halos[NUMHALOS - 1].id, is_equal_to(NUMHALOS - 1));
  assert_that(r->halos[0].num_p, is_equal_to(NUMPARTS));
  assert_that(r->halos[NUMHALOS - 1].num_p, is_equal_to(NUMPARTS));
}


TestSuite *load_rockstar_bin_tests()
{
  TestSuite *suite = create_test_suite();
  add_test_with_context(suite, load_rockstar_bin, loads_halo_data_properly);
  return suite;
}
