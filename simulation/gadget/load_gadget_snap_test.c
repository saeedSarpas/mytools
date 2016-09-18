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
#define DUMMYMASS 1.23456
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
  FILE *snapshot;
  snapshot = fopen(GADGET_SNAP_ADDR, "wb");

  int _dummy_int = 1;
#define SKIPINT fwrite(&_dummy_int, sizeof(_dummy_int), 1, snapshot)

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

  int ntot_withmasses = 0;
  for(i = 0; i < 6; i++)
    if(gh.mass[i] == 0)
      ntot_withmasses += gh.npart[i];

  SKIPINT;
  fwrite(&gh, sizeof(struct gadget_header), 1, snapshot);
  SKIPINT;

  /* Filling positions */
  float pos[3] = {XPOS, YPOS, ZPOS};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&pos, sizeof(float), 3, snapshot);
  SKIPINT;

  /* Filling velocities */
  float vel[3] = {XVEL, YVEL, ZVEL};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&vel, sizeof(float), 3, snapshot);
  SKIPINT;

  /* Filling ids */
  int temp_id, offset = 0;
  SKIPINT;
  for(i = 0; i < 6; i++){
    for(j = 0; j < gh.npart[i]; j++){
      temp_id = (gh.npart[i] - 1) - j + offset; // Reverse the order of ids
      fwrite(&temp_id, sizeof(int), 1, snapshot);
    }
    offset += gh.npart[i];
  }
  SKIPINT;

  if(ntot_withmasses > 0) SKIPINT;

  float dummy_mass = DUMMYMASS;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      if(gh.mass[i] == 0)
        fwrite(&dummy_mass, sizeof(float), 1, snapshot);

  if(ntot_withmasses > 0) SKIPINT;

  fclose(snapshot);
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
    for(j = 0; j < g->headers->npart[i]; j += 5){
      assert_that(g->particles[j + offset].id, is_equal_to(j + offset));
      assert_that(g->particles[j + offset].Pos[0], is_equal_to(XPOS));
      assert_that(g->particles[j + offset].Pos[1], is_equal_to(YPOS));
      assert_that(g->particles[j + offset].Pos[2], is_equal_to(ZPOS));
      assert_that(g->particles[j + offset].Vel[0], is_equal_to(XVEL));
      assert_that(g->particles[j + offset].Vel[1], is_equal_to(YVEL));
      assert_that(g->particles[j + offset].Vel[2], is_equal_to(ZVEL));
      if(g->headers->mass[i] == 0.0)
        assert_that(g->particles[j + offset].Mass, is_equal_to(DUMMYMASS));
      else
        assert_that(g->particles[j + offset].Mass,
                    is_equal_to(g->headers->mass[i]));
    }
    offset += g->headers->npart[i];
  }
}


Ensure(load_gadget_snap, works_with_a_real_snapshot)
{
  FILE *fp = fopen("./z1_2LPT_8_1kpc.dat", "rb");
  struct gadget *g = load_gadget_snap(fp);
  fclose(fp);

  assert_that(g->headers->npart[0], is_equal_to(0));
  assert_that(g->headers->npart[1], is_equal_to(8 * 8 * 8));
  assert_that(g->headers->npart[2], is_equal_to(0));
  assert_that(g->headers->npart[3], is_equal_to(0));
  assert_that(g->headers->npart[4], is_equal_to(0));
  assert_that(g->headers->npart[5], is_equal_to(0));
  assert_that_double(g->headers->redshift, is_equal_to_double(1));
  assert_that_double(g->headers->BoxSize, is_equal_to_double(1.0));
  assert_that(g->headers->num_files, is_equal_to(1));
  assert_that_double(g->headers->Omega0, is_equal_to_double(0.3089));
  assert_that_double(g->headers->OmegaLambda, is_equal_to_double(0.6911));
  assert_that_double(g->headers->HubbleParam, is_equal_to_double(0.6774));

  int i, j, offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j += 11){
      struct particle tmp_part = g->particles[j + offset];
      assert_true(0 <= tmp_part.id && tmp_part.id < 8 * 8 * 8);
      assert_true(-1 <= tmp_part.Pos[0] && tmp_part.Pos[0] <= 1);
      assert_true(-1 <= tmp_part.Pos[1] && tmp_part.Pos[1] <= 1);
      assert_true(-1 <= tmp_part.Pos[2] && tmp_part.Pos[2] <= 1);
      assert_true(-1 <= tmp_part.Vel[0] && tmp_part.Vel[0] <= 1);
      assert_true(-1 <= tmp_part.Vel[1] && tmp_part.Vel[1] <= 1);
      assert_true(-1 <= tmp_part.Vel[2] && tmp_part.Vel[2] <= 1);
      if(g->headers->mass[i] == 0.0)
        assert_that(tmp_part.Mass, is_equal_to(DUMMYMASS));
      else
        assert_that(tmp_part.Mass, is_equal_to(g->headers->mass[i]));
    }
    offset += g->headers->npart[i];
  }
}
