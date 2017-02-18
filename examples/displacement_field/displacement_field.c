/*
 * examples/displacement_field/displacement_field.c
 *
 * Generating a displacement field and save it to disk
 *
 * argv arguments:
 * snapshot_path: path to the snapshot
 * resolution: e.g. 256, 512, 1024, etc.
 * formatted_output_path: e.g. "./output/displacement_field_%03d"
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./displacement_field.h"
#include "./../../configfile/mylibconfig.h"
#include "./../../memory/allocate.h"
#include "./../../io/open_file.h"
#include "./../../simulation/snapshot.h"
#include "./../../simulation/gadget/load_gadget_snap.h"
#include "./../../simulation/load_snap_from.h"
#include "./../../simulation/save_snap_to.h"
#include "./../../simulation/check_if_saved_exist.c"
#include "./../../time/mytime.h"
#include "./../../grid/row_major_order/three_to_one.h"


static displacement_field_params* loadconfig(const char*);
static snapshot* safe_snapshot_load(char*);
int cast_to_int(const char*);


int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("[Error] You should specify the paths to the config file\n");
    printf("        ./displacement_field /path/to/config.file\n");
    exit(EXIT_FAILURE);
  }

  // Loading configs
  displacement_field_params *p = loadconfig(argv[1]);

  // Loading the snapshot
  clock_t _l_s_ = start("Loading the snapshot\n");
  snapshot *snaps[3];
  snaps[0] = safe_snapshot_load(p->snap[0]);
  snaps[1] = safe_snapshot_load(p->snap[1]);
  snaps[2] = safe_snapshot_load(p->snap[2]);
  done(_l_s_);

  clock_t _w_o_ = start("Writing output files");
  int i, j, k, index,
    resolutions[3] = {256, 512, 1024},
    factors[3] = {1, 2, 4};


  for(i = 0; i < 3; i++){
    int dims[3] = {resolutions[i], resolutions[i], resolutions[i]};

    index = three_to_one(factors[i] * p->lowResX,
                         factors[i] * p->lowResY,
                         factors[i] * p->lowResZ, dims);

    FILE *file = open_file(p->ascii[i], "w");

    for(j = 0; j < factors[i] * p->lowResBoxLen; j++){
      for(k = 0; k < factors[i] * p->lowResBoxLen; k++){
        fprintf(file, "%d\t%8.6e\t%8.6e\t%8.6e\n",
                (int)snaps[i]->particles[index + k].id,
                snaps[i]->particles[index + k].pos[0] / 1000,
                snaps[i]->particles[index + k].pos[1] / 1000,
                snaps[i]->particles[index + k].pos[2] / 1000);
      }

      index += resolutions[i];
    }

    fclose(file);
  }
  done(_w_o_);

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


static displacement_field_params* loadconfig(const char *path)
{
  displacement_field_params *p = allocate(1, sizeof(*p));

  config_t *cfg = new_cfg();
  cfg_loadfile(cfg, path);

  config_setting_t *inputs = cfg_findsetting(cfg, "inputs");

  const char *input = cfg_getstring(inputs, "snap_256");
  p->snap[0] = strdup(input);

  input = cfg_getstring(inputs, "snap_512");
  p->snap[1] = strdup(input);

  input = cfg_getstring(inputs, "snap_1024");
  p->snap[2] = strdup(input);

  config_setting_t *options = cfg_findsetting(cfg, "options");

  p->lowResX = cfg_getint(options, "low_res_x");
  p->lowResY = cfg_getint(options, "low_res_y");
  p->lowResZ = cfg_getint(options, "low_res_z");
  p->lowResBoxLen = cfg_getint(options, "low_res_box_len");

  config_setting_t *outputs = cfg_findsetting(cfg, "outputs");

  const char *output = cfg_getstring(outputs, "ascii_256");
  p->ascii[0] = strdup(output);

  output = cfg_getstring(outputs, "ascii_512");
  p->ascii[1] = strdup(output);

  output = cfg_getstring(outputs, "ascii_1024");
  p->ascii[2] = strdup(output);

  return p;
}
