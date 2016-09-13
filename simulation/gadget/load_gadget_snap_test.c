/*
 * simulation/load_gadget_snap_test.c
 * test_for: simulation/load_gadget_snap.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "load_gadget_snap.h"
#include "./../shared_simulation_data_type.h"
#include "./../../avltree/avl_tree.h"


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
      temp_id = j + offset;
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
  struct avl_node *found_node;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i]; j += 5){
      found_node = avl_find(g->particles, j + offset);
      assert_that(((struct particle*)(found_node->data))->id,
                  is_equal_to(j + offset));
      assert_that(((struct particle*)(found_node->data))->Pos[0],
                  is_equal_to(XPOS));
      assert_that(((struct particle*)(found_node->data))->Pos[1],
                  is_equal_to(YPOS));
      assert_that(((struct particle*)(found_node->data))->Pos[2],
                  is_equal_to(ZPOS));
      assert_that(((struct particle*)(found_node->data))->Vel[0],
                  is_equal_to(XVEL));
      assert_that(((struct particle*)(found_node->data))->Vel[1],
                  is_equal_to(YVEL));
      assert_that(((struct particle*)(found_node->data))->Vel[2],
                  is_equal_to(ZVEL));
      if(g->headers->mass[i] == 0.0)
        assert_that(((struct particle*)(found_node->data))->Mass,
                    is_equal_to(DUMMYMASS));
      else
        assert_that(((struct particle*)(found_node->data))->Mass,
                    is_equal_to(g->headers->mass[i]));
    }
    offset += g->headers->npart[i];
  }

  avl_dispose(g->particles);

  free(g);
}
