/*
 * simulation/gadget/load_snap_from.c
 * test_file: simulation/gadget/save_snap_to_test.c
 *
 * Loading structured Gadget snapshot
 *
 * param: path path to the output file
 *
 * author: Saeed Sarpas
 */


#include "./load_snap_from.h"
#include "./../memory/allocate.h"
#include "./../io/open_file.h"
#include "./../io/read_from.h"


snapshot* load_snap_from(char *path)
{
  snapshot *snap = allocate(1, sizeof(*snap));

  FILE *file = open_file(path, "rb");

  snap->header = allocate(1, sizeof(snapshotheader));
  read_from(file, 1, sizeof(snapshotheader), snap->header);

  snap->particles = allocate(snap->header->tot_nparticles,
                             sizeof(snapshotparticle));
  read_from(file, snap->header->tot_nparticles,
            sizeof(snapshotparticle), snap->particles);

  fclose(file);
  return snap;
}
