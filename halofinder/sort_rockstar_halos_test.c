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

  /* Filling headers */
  struct rockstar_header rh;
  rh.num_halos = NUMHALOS;
  rh.num_particles = NUMHALOS * NUMPARTS;
  fwrite(&rh, sizeof(struct rockstar_header), 1, file);

  /* Filling halos info */
  int i;
  for (i = NUMHALOS; i > 0; i--) {
    int index = NUMHALOS - i;
    int p_start =  index * NUMPARTS;

    struct rockstar_halo h;
    h.id = i;
    h.m = i * 10e10;
    h.num_p = NUMPARTS;
    h.p_start = p_start;
    fwrite(&h, sizeof(struct rockstar_halo), 1, file);
  }

  /* Filling particle ids */
  for (i = 0; i < NUMHALOS; i++)
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
  struct rockstar *r = load_rockstar_bin(fp);
  fclose(fp);

  sort_rockstar_halos(r->halos, r->headers->num_halos, "m");

  int i;
  for (i = 1; i < NUMHALOS; i++)
    assert_true(r->halos[i].m >= r->halos[i-1].m);
}

Ensure(sort_rockstar_halos, sorts_halos_properly_based_on_their_id)
{
  FILE *fp = fopen(ROCKSTAR_FILE_ADDR, "rb");
  struct rockstar *r = load_rockstar_bin(fp);
  fclose(fp);

  sort_rockstar_halos(r->halos, r->headers->num_halos, "id");

  int i;
  for (i = 1; i < NUMHALOS; i++)
    assert_true(r->halos[i].id >= r->halos[i-1].id);
}


TestSuite *sort_rockstar_halos_tests()
{
  TestSuite *suite = create_test_suite();
  add_test_with_context(suite, sort_rockstar_halos,
                        sorts_halos_properly_based_on_their_mass);
  add_test_with_context(suite, sort_rockstar_halos,
                        sorts_halos_properly_based_on_their_id);
  return suite;
}