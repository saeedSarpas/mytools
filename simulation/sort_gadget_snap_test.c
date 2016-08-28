/*
 * simulation/sort_gadget_snap_test.c
 * test_for: simulation/sort_gadget_snap.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "load_gadget_snap.h"
#include "sort_gadget_snap.h"


Describe(sort_gadget_snap);


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


BeforeEach(sort_gadget_snap)
{
  int i, j, k;
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

  SKIPINT;
  srand(1234);

  for(i = 0; i < 6; i++)
    if(gh.npart[i] != 0){
      /* Filling ids */
      int *ids = malloc(gh.npart[i] * sizeof(int));

      for(j = 0; j < gh.npart[i]; j++)
        ids[j] = j;

      int temp_index, ntot = gh.npart[i];

      for(j = 0; j < gh.npart[i]; j++){
        temp_index = rand() % ntot;
        fwrite(&ids[temp_index], sizeof(int), 1, file);

        /* Deleting id from ids */
        for(k = temp_index; k < ntot - 1; k++)
          ids[k] = ids[k+1];
        ntot--;
      }

      free(ids);
    }
  SKIPINT;

  fclose(file);
}


AfterEach(sort_gadget_snap)
{
  remove(GADGET_SNAP_ADDR);
}


Ensure(sort_gadget_snap, sorts_snapshot_based_on_ids)
{
  FILE *fp = fopen(GADGET_SNAP_ADDR, "rb");
  struct gadget *g = load_gadget_snap(fp);
  fclose(fp);

  sort_gadget_snap(g);

  int i, j, offset = 0;
  for(i = 0; i < 6; i++){
    for(j = 0; j < g->headers->npart[i] - 1; j++)
      assert_true(
        g->particles[j + offset + 1].id >= g->particles[j + offset].id);
    offset += g->headers->npart[i];
  }
}


TestSuite *sort_gadget_snap_tests()
{
  TestSuite *suite = create_test_suite();
  add_test_with_context(suite, sort_gadget_snap, sorts_snapshot_based_on_ids);
  return suite;
}
