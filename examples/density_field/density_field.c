/*
 * examples/density_field/density_field.c
 *
 * Generating a density field and save it to disk
 *
 * argv arguments:
 * snapshot_path: path to the snapshot
 * resolution: e.g. 256, 512, 1024, etc.
 * formatted_output_path: e.g. "./output/density_field_%03d"
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include "./../../simulation/snapshot.h"
#include "./../../simulation/gadget/load_gadget_snap.h"
#include "./../../simulation/load_snap_from.h"
#include "./../../simulation/save_snap_to.h"
#include "./../../simulation/check_if_saved_exist.c"
#include "./../../io/open_file.h"
#include "./../../massassignmentfunctions/cic/cic.h"
#include "./../../time/mytime.h"
#include "./../../loading/simple_loading.h"


static snapshot* safe_snapshot_load(char*);
int cast_to_int(const char*);
void density_contrast(double*, int*, double*);


int main(int argc, char *argv[])
{
  if(argc != 4){
    printf("[Error] You should specify 3 parameters as inputs..\n");
    printf("        ./density_field /path/to/snapshot 1024 /formatted/output/path\n");
    exit(EXIT_FAILURE);
  }

  // Loading the snapshot
  clock_t _l_s_ = start("Loading the snapshot\n");
  snapshot *snap = safe_snapshot_load(argv[1]);
  done(_l_s_);

  // Casting the second (resolution) argument to integer
  int res = cast_to_int(argv[2]);

  // Calculating the density contrast
  clock_t _c_d_ = start("Calculating the density contrast");
  int grid_dims[3] = {res, res, res};

  double box_lengths[3] = {
    snap->header->boxsize,
    snap->header->boxsize,
    snap->header->boxsize};

  double *density_field = cic(snap, grid_dims, box_lengths);
  density_contrast(density_field, grid_dims, box_lengths);
  done(_c_d_);

  // Saving results on disk
  clock_t _s_r_ = start("Saving results on dist\n");
  int i, j, k, percent = -1;
  char output_addr[1024];

  for(i = 0; i < res; i++){
    percent = simple_loading(percent, i, res);

    sprintf(output_addr, argv[3], i);
    FILE* file = open_file(output_addr, "w");

    for(j = 0; j < res; j++){
      for(k = 0; k < res; k++)
        fprintf(file, "%14.12e\t", density_field[i * res * res + j * res + k]);

      fprintf(file, "\n");
    }
    fclose(file);
  }
  done(_s_r_);

  dispose_snapshot(&snap);
  free(density_field);

  return 0;
}


static snapshot* safe_snapshot_load(char *path)
{
  if(check_if_saved_exist(path)){
    char msg[1024];
    sprintf(msg, "\t[Saved version exist for %s]", path);
    clock_t _s_e_ = start(msg);
    snapshot *snap = load_snap_from(saved_snap_path(path));
    done(_s_e_);
    return snap;
  }
  else{
    char msg[1024];
    sprintf(msg, "\t[Loading %s]", path);
    clock_t _s_e_ = start(msg);
    snapshot *snap = load_gadget_snap(path);
    save_snap_to(saved_snap_path(path), snap);
    done(_s_e_);
    return snap;
  }
}


int cast_to_int(const char *str)
{
  char *p;
  long conv = strtol(str, &p, 10);

  if(errno != 0 || *p != '\0' || conv > INT_MAX){
    printf("[Error] Cannot cast the second argument to integer.");
    exit(EXIT_FAILURE);
  } else
    return (int)conv;
}


void density_contrast(double *grid, int *dims, double *box)
{
  double tot_mass = 0.0;
  double grid_vol = pow(box[0] / dims[0], 3);
  int i, ntot = dims[0] * dims[1] * dims[2];

  for(i = 0; i < ntot; i++)
    tot_mass += grid[i];

  double mean_rho = tot_mass / (box[0] * box[1] * box[2]);

  for(i = 0; i < ntot; i++)
    grid[i] =((grid[i] / grid_vol) / mean_rho) - 1;
}
