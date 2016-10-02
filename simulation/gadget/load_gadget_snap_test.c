/*
 * simulation/gadget/load_gadget_snap_test.c
 * test_for: simulation/gadget/load_gadget_snap.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "load_gadget_snap.h"
#include "gadget_data_type.h"


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

// Filling headers
  gadgetheader gh;
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
  fwrite(&gh, sizeof(gadgetheader), 1, snapshot);
  SKIPINT;

// Filling positions
  float pos[3] = {XPOS, YPOS, ZPOS};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&pos, sizeof(float), 3, snapshot);
  SKIPINT;

// Filling velocities
  float vel[3] = {XVEL, YVEL, ZVEL};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&vel, sizeof(float), 3, snapshot);
  SKIPINT;

// Filling ids
  int temp_id;
  SKIPINT;
  for(i = 0; i < 6; i++){
    for(j = 0; j < gh.npart[i]; j++){
      temp_id = (gh.npart[i] - 1) - j; // Reverse the order of ids
      fwrite(&temp_id, sizeof(int), 1, snapshot);
    }
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
  snapshot *s = load_gadget_snap(fp);
  fclose(fp);

  assert_that_double(s->header->mdarkpart, is_equal_to_double(HALOMASS));
  assert_that(s->header->mgaspart, is_equal_to(0));
  assert_that_double(s->header->mstarpart, is_equal_to_double(STARMASS));

  int i;
  for(i = 0; i < s->header->ndarkpart; i += 3){
    assert_that(s->darkparts[i].id, is_equal_to(s->header->ndarkpart -1 - i));
    assert_that(s->darkparts[i].Pos[0], is_equal_to(XPOS));
    assert_that(s->darkparts[i].Pos[1], is_equal_to(YPOS));
    assert_that(s->darkparts[i].Pos[2], is_equal_to(ZPOS));
    assert_that(s->darkparts[i].Vel[0], is_equal_to(XVEL));
    assert_that(s->darkparts[i].Vel[1], is_equal_to(YVEL));
    assert_that(s->darkparts[i].Vel[2], is_equal_to(ZVEL));
  }

  for(i = 0; i < s->header->nstarpart; i += 3){
    assert_that(s->starparts[i].id, is_equal_to(s->header->nstarpart -1 - i));
    assert_that(s->starparts[i].Pos[0], is_equal_to(XPOS));
    assert_that(s->starparts[i].Pos[1], is_equal_to(YPOS));
    assert_that(s->starparts[i].Pos[2], is_equal_to(ZPOS));
    assert_that(s->starparts[i].Vel[0], is_equal_to(XVEL));
    assert_that(s->starparts[i].Vel[1], is_equal_to(YVEL));
    assert_that(s->starparts[i].Vel[2], is_equal_to(ZVEL));
  }
}


Ensure(load_gadget_snap, works_with_a_real_snapshot)
{
  FILE *fp = fopen("./z1_2LPT_8_1kpc.dat", "rb");
  snapshot *s = load_gadget_snap(fp);
  fclose(fp);

  assert_that(s->header->ndarkpart, is_equal_to(8 * 8 * 8));
  assert_that(s->header->ngaspart, is_equal_to(0));
  assert_that(s->header->nstarpart, is_equal_to(0));

  assert_that(s->header->mgaspart, is_equal_to(0));
  assert_that(s->header->mstarpart, is_equal_to(0));

  assert_that_double(s->header->redshift, is_equal_to_double(1));

  assert_that_double(s->header->boxsize, is_equal_to_double(1.0));

  assert_that_double(s->header->Om, is_equal_to_double(0.3089));
  assert_that_double(s->header->Ol, is_equal_to_double(0.6911));
  assert_that_double(s->header->h0, is_equal_to_double(0.6774));

  int i;
    for(i = 0; i < s->header->ndarkpart; i += 11){
      snapshotparticle tmp_part = s->darkparts[i];
      assert_true(0 <= tmp_part.id && tmp_part.id < 8 * 8 * 8);
      assert_true(-1 <= tmp_part.Pos[0] && tmp_part.Pos[0] <= 1);
      assert_true(-1 <= tmp_part.Pos[1] && tmp_part.Pos[1] <= 1);
      assert_true(-1 <= tmp_part.Pos[2] && tmp_part.Pos[2] <= 1);
      assert_true(-1 <= tmp_part.Vel[0] && tmp_part.Vel[0] <= 1);
      assert_true(-1 <= tmp_part.Vel[1] && tmp_part.Vel[1] <= 1);
      assert_true(-1 <= tmp_part.Vel[2] && tmp_part.Vel[2] <= 1);
    }
}
