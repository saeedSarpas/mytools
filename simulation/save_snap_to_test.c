/*
 * simulation/gadget/save_snap_to_test.c
 * test_for: simulation/gadget/save_snap_to.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include "./save_snap_to.h"
#include "./load_snap_from.h"
#include "./snapshot.h"
#include "./../memory/allocate.h"


#define GADGET_SNAP_ADDR "./structured_snapshot.bin"
#define TOTNUMPARTS 7
#define MASS 1.23e4


snapshot *filled_snap = NULL;
snapshotparticle *particles = NULL;
float POS[3] = {1.23, 2.34, 3.45};
float VEL[3] = {4.56, 5.67, 6.78};


Describe(save_snap_to);


BeforeEach(save_snap_to)
{
  filled_snap = allocate(1, sizeof(*filled_snap));
  filled_snap->header = allocate(1, sizeof(snapshotheader));
  filled_snap->header->tot_nparticles = TOTNUMPARTS;

  filled_snap->particles = allocate(TOTNUMPARTS, sizeof(snapshotparticle));

  int i;
  for(i = 0; i < TOTNUMPARTS; i++){
    filled_snap->particles[i].id = i;
    filled_snap->particles[i].pos[0] = POS[0];
    filled_snap->particles[i].pos[1] = POS[1];
    filled_snap->particles[i].pos[2] = POS[2];
    filled_snap->particles[i].vel[0] = VEL[0];
    filled_snap->particles[i].vel[1] = VEL[1];
    filled_snap->particles[i].vel[2] = VEL[2];
    filled_snap->particles[i].mass = MASS;
  }
}


AfterEach(save_snap_to)
{
  dispose_snapshot(filled_snap);
  remove(GADGET_SNAP_ADDR);
}


Ensure(save_snap_to, saves_gadget_snapshot_on_disk_properly)
{
  save_snap_to(GADGET_SNAP_ADDR, filled_snap);

  snapshot *snap = load_snap_from(GADGET_SNAP_ADDR);

  assert_that(snap->header->tot_nparticles, is_equal_to(TOTNUMPARTS));

  int i;
  for(i = 0; i < TOTNUMPARTS; i++){
    assert_that(snap->particles[i].id, is_equal_to(i));
    assert_that_double(snap->particles[i].pos[0], is_equal_to_double(POS[0]));
    assert_that_double(snap->particles[i].pos[1], is_equal_to_double(POS[1]));
    assert_that_double(snap->particles[i].pos[2], is_equal_to_double(POS[2]));
    assert_that_double(snap->particles[i].vel[0], is_equal_to_double(VEL[0]));
    assert_that_double(snap->particles[i].vel[1], is_equal_to_double(VEL[1]));
    assert_that_double(snap->particles[i].vel[2], is_equal_to_double(VEL[2]));
    assert_that_double(snap->particles[i].mass, is_equal_to_double(MASS));
  }
}
