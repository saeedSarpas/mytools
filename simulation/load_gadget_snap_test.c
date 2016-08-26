/*
 * simulation/load_gadget_snap_test.c
 * test_for: simulation/load_gadget_snap.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "load_gadget_snap.h"


Describe(load_gadget_snap);


#define GADGET_SNAP_ADDR "./rockstar.bin"
#define NUMHALOPARTS 10
#define HALOMASS 1E10
#define NUMSTARS 100
#define STARMASS 1E6
#define XPOS 1.23
#define YPOS 4.56
#define ZPOS 7.89
#define XVEL -1.23
#define YVEL -4.56
#define ZVEL -7.89


BeforeEach(load_gadget_snap)
{
  int i, j;
  FILE *file;
  file = fopen(GADGET_SNAP_ADDR, "wb");

  int _dummy_int = 1;
#define SKIPINT fwrite(&_dummy_int, sizeof(_dummy_int), 1, file)

  /* Filling headers */
  struct gadget_header gh;
  for(i = 0; i < 6; i++) {
    gh.npart[i] = 0;
    gh.mass[i] = 0.0;
  }
  gh.npart[1] = NUMHALOPARTS;
  gh.mass[1] = HALOMASS;
  gh.npart[4] = NUMSTARS;
  gh.mass[4] = STARMASS;

  SKIPINT;
  fwrite(&gh, sizeof(struct gadget_header), 1, file);
  SKIPINT;

  /* Filling positions */
  float pos[3] = {XPOS, YPOS, ZPOS};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&pos, sizeof(float), 3, file);
  SKIPINT;

  /* Filling velocities */
  float vel[3] = {XVEL, YVEL, ZVEL};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&vel, sizeof(float), 3, file);
  SKIPINT;

  /* Filling ids */
  int temp_id, offset = 0;
  SKIPINT;
  for(i = 0; i < 6; i++){
    for(j = 0; j < gh.npart[i]; j++){
      temp_id = j + offset;
      fwrite(&temp_id, sizeof(int), 1, file);
    }
    offset += gh.npart[i];
  }
  SKIPINT;

  fclose(file);
}


AfterEach(load_gadget_snap)
{
  remove(GADGET_SNAP_ADDR);
}


Ensure(load_gadget_snap, loads_snapshot_properly)
{
  FILE *fp = fopen(GADGET_SNAP_ADDR, "rb");
  struct gadget *g = load_gadget_snap(fp);
  fclose(fp);

  int i, j, offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j++){
      assert_that(g->particles[j + offset].id, is_equal_to(j + offset));
      assert_that(g->particles[j + offset].Pos[0], is_equal_to(XPOS));
      assert_that(g->particles[j + offset].Pos[1], is_equal_to(YPOS));
      assert_that(g->particles[j + offset].Pos[2], is_equal_to(ZPOS));
      assert_that(g->particles[j + offset].Vel[0], is_equal_to(XVEL));
      assert_that(g->particles[j + offset].Vel[1], is_equal_to(YVEL));
      assert_that(g->particles[j + offset].Vel[2], is_equal_to(ZVEL));
    }
    offset += g->headers->npart[i];
  }
}


TestSuite *load_gadget_snap_tests()
{
  TestSuite *suite = create_test_suite();
  add_test_with_context(suite, load_gadget_snap, loads_snapshot_properly);
  return suite;
}
