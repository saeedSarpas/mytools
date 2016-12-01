/*
 * simulation/gadget/save_snap_to.c
 * test_file: simulation/gadget/save_snap_to_test.c
 *
 * Saving structured Gadget snapshot
 *
 * param: snapshotfile Gadget snapshot file
 * param: path path to the output file
 *
 * author: Saeed Sarpas
 */


#include "./save_snap_to.h"
#include "./../io/open_file.h"
#include "./../io/write_to.h"


void save_snap_to(char *path, snapshot *snap)
{
  FILE *file = open_file(path, "wb");
  write_to(file, 1, sizeof(snapshotheader), snap->header);
  write_to(file, snap->header->tot_nparticles, sizeof(snapshotparticle),
           snap->particles);

  fclose(file);
}
