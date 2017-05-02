/**
 * examples/rg_matchfinder/unmatched/unmatched_halos_config.c
 *
 * Loading a configuration file for generating unmatched halos
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <string.h>
#include "unmatched_halos_config.h"
#include "./../../../memory/allocate.h"
#include "./../../../configfile/mylibconfig.h"
#include "./../../../io/check_file.h"
#include "./../../../io/check_dir.h"


static void check_file_harsh(char*, char*);


unmatched_halos_params* load_unmatched_halos_params(const char *path)
{
  unmatched_halos_params* p = allocate(1, sizeof(*p));

  config_t* cfg = new_cfg();
  cfg_loadfile(cfg, path);

  config_setting_t* inputs = cfg_findsetting(cfg, "inputs");

  const char* input = cfg_getstring(inputs, "matched_catalog");
  p->matched_catalog = strdup(input);
  check_file_harsh(p->matched_catalog, "[Error] Matched catalog does not exist");

  input = cfg_getstring(inputs, "pri_halos");
  p->pri_halos = strdup(input);
  check_file_harsh(p->pri_halos, "[Error] Primary halos does not exist");

  input = cfg_getstring(inputs, "pri_snap");
  p->pri_snap = strdup(input);
  check_file_harsh(p->pri_snap, "[Error] Primary snapshot does not exist");

  input = cfg_getstring(inputs, "sec_snap");
  p->sec_snap = strdup(input);
  check_file_harsh(p->sec_snap, "[Error] Secondary snapshot does not exist");

  p->box_size = cfg_getint(inputs, "box_size");

  config_setting_t* outputs = cfg_findsetting(cfg, "outputs");

  const char* output_dir = cfg_getstring(outputs, "output_dir");
  p->output_dir = strdup(output_dir);
  if (check_dir(p->output_dir) != CD_DIR_EXIST)
    {
      printf("[Error] Output directory does not exitst");
      exit(EXIT_FAILURE);
    }

  return p;
}


static void check_file_harsh(char *path, char *error)
{
  if(check_file(path) != CF_FILE_EXIST){
    printf("[Error! %s]\n", error);
    exit(EXIT_FAILURE);
  }
}
