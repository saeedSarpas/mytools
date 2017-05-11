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


  input = cfg_getstring(inputs, "halos256");
  p->halos256 = strdup(input);
  check_file_harsh(p->halos256, "[Error] 256-res halos does not exist");

  input = cfg_getstring(inputs, "halos512");
  p->halos512 = strdup(input);
  check_file_harsh(p->halos512, "[Error] 512-res halos does not exist");

  input = cfg_getstring(inputs, "halos1024");
  p->halos1024 = strdup(input);
  check_file_harsh(p->halos1024, "[Error] 1024-res halos does not exist");


  input = cfg_getstring(inputs, "snap256");
  p->snap256 = strdup(input);
  check_file_harsh(p->snap256, "[Error] 256-res snapshot does not exist");

  input = cfg_getstring(inputs, "snap512");
  p->snap512 = strdup(input);
  check_file_harsh(p->snap512, "[Error] 512-res snapshot does not exist");

  input = cfg_getstring(inputs, "snap1024");
  p->snap1024 = strdup(input);
  check_file_harsh(p->snap1024, "[Error] 1024-res snapshot does not exist");


  p->window_len = cfg_getint(inputs, "window_len");


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
