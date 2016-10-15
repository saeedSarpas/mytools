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
  FILE *snapshotfile;
  snapshotfile = fopen(GADGET_SNAP_ADDR, "wb");

  int _dummy_int = 1;
#define SKIPINT fwrite(&_dummy_int, sizeof(_dummy_int), 1, snapshotfile)

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
  fwrite(&gh, sizeof(gadgetheader), 1, snapshotfile);
  SKIPINT;

// Filling positions
  float pos[3] = {XPOS, YPOS, ZPOS};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&pos, sizeof(float), 3, snapshotfile);
  SKIPINT;

// Filling velocities
  float vel[3] = {XVEL, YVEL, ZVEL};
  SKIPINT;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      fwrite(&vel, sizeof(float), 3, snapshotfile);
  SKIPINT;

// Filling ids
  int temp_id, offset = 0;
  SKIPINT;
  for(i = 0; i < 6; i++){
    for(j = 0; j < gh.npart[i]; j++){
      temp_id = (gh.npart[i] - 1) - j + offset; // Reverse the order of ids
      fwrite(&temp_id, sizeof(int), 1, snapshotfile);
    }
    offset += gh.npart[i];
  }
  SKIPINT;

  if(ntot_withmasses > 0) SKIPINT;

  float dummy_mass = DUMMYMASS;
  for(i = 0; i < 6; i++)
    for(j = 0; j < gh.npart[i]; j++)
      if(gh.mass[i] == 0)
        fwrite(&dummy_mass, sizeof(float), 1, snapshotfile);

  if(ntot_withmasses > 0) SKIPINT;

  fclose(snapshotfile);
}


AfterEach(load_gadget_snap)
{
  remove(GADGET_SNAP_ADDR);
}


Ensure(load_gadget_snap, loads_snapshot_properly)
{
  snapshot *s = load_gadget_snap(GADGET_SNAP_ADDR);

  assert_that(s->header->npart[0], is_equal_to(0));
  assert_that(s->header->npart[1], is_equal_to(NUMHALOPARTS));
  assert_that(s->header->npart[2], is_equal_to(0));
  assert_that(s->header->npart[3], is_equal_to(0));
  assert_that(s->header->npart[4], is_equal_to(NUMSTARS));
  assert_that(s->header->npart[5], is_equal_to(0));

  assert_that_double(s->header->tot_nparticles,
                     is_equal_to_double(NUMHALOPARTS + NUMSTARS));

  assert_that_double(s->header->mass[0], is_equal_to_double(0));
  assert_that_double(s->header->mass[1], is_equal_to_double(HALOMASS));
  assert_that_double(s->header->mass[2], is_equal_to_double(0));
  assert_that_double(s->header->mass[3], is_equal_to_double(0));
  assert_that_double(s->header->mass[4], is_equal_to_double(STARMASS));
  assert_that_double(s->header->mass[5], is_equal_to_double(0));

  int i, j, offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < s->header->npart[i]; j += 3){
      assert_that(s->particles[j + offset].id, is_equal_to(j + offset));
      assert_that(s->particles[j + offset].type, is_equal_to(i));
      assert_that(s->particles[j + offset].pos[0], is_equal_to(XPOS));
      assert_that(s->particles[j + offset].pos[1], is_equal_to(YPOS));
      assert_that(s->particles[j + offset].pos[2], is_equal_to(ZPOS));
      assert_that(s->particles[j + offset].vel[0], is_equal_to(XVEL));
      assert_that(s->particles[j + offset].vel[1], is_equal_to(YVEL));
      assert_that(s->particles[j + offset].vel[2], is_equal_to(ZVEL));
    }
    offset += s->header->npart[i];
  }
}


Ensure(load_gadget_snap, works_with_a_real_snapshot)
{
  snapshot *s = load_gadget_snap("./z1_2LPT_8_1kpc.dat");

  assert_that(s->header->npart[0], is_equal_to(0));
  assert_that(s->header->npart[1], is_equal_to(8 * 8 * 8));
  assert_that(s->header->npart[2], is_equal_to(0));
  assert_that(s->header->npart[3], is_equal_to(0));
  assert_that(s->header->npart[4], is_equal_to(0));
  assert_that(s->header->npart[5], is_equal_to(0));

  assert_that(s->header->mass[0], is_equal_to(0));
  assert_that(s->header->mass[1], is_equal_to(0));
  assert_that(s->header->mass[2], is_equal_to(0));
  assert_that(s->header->mass[3], is_equal_to(0));
  assert_that(s->header->mass[4], is_equal_to(0));
  assert_that(s->header->mass[5], is_equal_to(0));

  assert_that_double(s->header->redshift, is_equal_to_double(1));

  assert_that_double(s->header->boxsize, is_equal_to_double(1.0));

  assert_that_double(s->header->Om, is_equal_to_double(0.3089));
  assert_that_double(s->header->Ol, is_equal_to_double(0.6911));
  assert_that_double(s->header->h0, is_equal_to_double(0.6774));

  int i;
  for(i = 0; i < s->header->npart[1]; i += 23){
    snapshotparticle tmp_part = s->particles[i];
    assert_true(0 <= tmp_part.id && tmp_part.id < 8 * 8 * 8);
    assert_that(tmp_part.type, is_equal_to(1));
    assert_true(-1 <= tmp_part.pos[0] && tmp_part.pos[0] <= 1);
    assert_true(-1 <= tmp_part.pos[1] && tmp_part.pos[1] <= 1);
    assert_true(-1 <= tmp_part.pos[2] && tmp_part.pos[2] <= 1);
    assert_true(-1 <= tmp_part.vel[0] && tmp_part.vel[0] <= 1);
    assert_true(-1 <= tmp_part.vel[1] && tmp_part.vel[1] <= 1);
    assert_true(-1 <= tmp_part.vel[2] && tmp_part.vel[2] <= 1);
  }
}
