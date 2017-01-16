/*
 * examples/rg_mmatchfinder/mh_history/history_params.c
 *
 * Loading and disposing parameters for history module
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "./history_config.h"
#include "./../../../memory/allocate.h"
#include "./../../../io/check_file.h"
#include "./../../../io/check_dir.h"
#include "./../../../configfile/mylibconfig.h"
#include "./../../../strings/concat/concat.h"


static void check_file_harsh(char*, char*);


mh_history_params* load_params(const char *path)
{
  mh_history_params *p = allocate(1, sizeof(*p));

  config_t *cfg = new_cfg();
  cfg_loadfile(cfg, path);

  const char *input;
  config_setting_t *inputs = cfg_findsetting(cfg, "inputs");

  input = cfg_getstring(inputs, "matches_512_256");
  p->matches_512_256 = strdup(input);
  check_file_harsh(p->matches_512_256, "Wrong matches_512_256 in configs");

  input = cfg_getstring(inputs, "matches_1024_512");
  p->matches_1024_512 = strdup(input);
  check_file_harsh(p->matches_1024_512, "Wrong matches_1024_512 in configs");

  const char *raw_matches_256_path = cfg_getstring(inputs, "matches_256");
  const char *raw_matches_512_path = cfg_getstring(inputs, "matches_512");
  const char *raw_matches_1024_path = cfg_getstring(inputs, "matches_1024");

  p->num_matches = cfg_getint(inputs, "num_of_match_files");
  p->matches_256 = allocate(p->num_matches, sizeof(char*));
  p->matches_512 = allocate(p->num_matches, sizeof(char*));
  p->matches_1024 = allocate(p->num_matches, sizeof(char*));

  int i;
  char match_fname[1024], *errormsg;

  for(i = 0; i < p->num_matches; i++){
    sprintf(match_fname, raw_matches_256_path, i, i+1);
    p->matches_256[i] = strdup(match_fname);
    errormsg = concat(2, match_fname, " does not exist");
    check_file_harsh(p->matches_256[i], errormsg);

    sprintf(match_fname, raw_matches_512_path, i, i+1);
    p->matches_512[i] = strdup(match_fname);
    errormsg = concat(2, match_fname, " does not exist");
    check_file_harsh(p->matches_512[i], errormsg);

    sprintf(match_fname, raw_matches_1024_path, i, i+1);
    p->matches_1024[i] = strdup(match_fname);
    errormsg = concat(2, match_fname, " does not exist");
    check_file_harsh(p->matches_1024[i], errormsg);
  }

  const char *raw_halos_256_path = cfg_getstring(inputs, "halos_256");
  const char *raw_halos_512_path = cfg_getstring(inputs, "halos_512");
  const char *raw_halos_1024_path = cfg_getstring(inputs, "halos_1024");

  p->num_halos = cfg_getint(inputs, "num_of_halo_files");
  p->halos_256 = allocate(p->num_halos, sizeof(char*));
  p->halos_512 = allocate(p->num_halos, sizeof(char*));
  p->halos_1024 = allocate(p->num_halos, sizeof(char*));

  char halo_fname[1024];

  for(i = 0; i < p->num_halos; i++){
    sprintf(halo_fname, raw_halos_256_path, i);
    p->halos_256[i] = strdup(halo_fname);
    errormsg = concat(2, halo_fname, " does not exist");
    check_file_harsh(p->halos_256[i], errormsg);

    sprintf(halo_fname, raw_halos_512_path, i);
    p->halos_512[i] = strdup(halo_fname);
    errormsg = concat(2, halo_fname, " does not exist");
    check_file_harsh(p->halos_512[i], errormsg);

    sprintf(halo_fname, raw_halos_1024_path, i);
    p->halos_1024[i] = strdup(halo_fname);
    errormsg = concat(2, halo_fname, " does not exist");
    check_file_harsh(p->halos_1024[i], errormsg);
  }

  const char *output;
  config_setting_t *outputs = cfg_findsetting(cfg, "outputs");

  output = cfg_getstring(outputs, "ascii");
  p->ascii_output = strdup(output);
  char *for_dir_check = strdup(output);
  if(check_dir(dirname(for_dir_check)) != CD_DIR_EXIST){
    printf("[Error! Wrong ascii_output dir in configs]\n");
    exit(EXIT_FAILURE);
  }

  return p;
}


/*
 * Disposing params structure
 */
void dispose_params(mh_history_params **p)
{
  free((*p)->matches_512_256);
  free((*p)->matches_1024_512);
  int i;
  for(i = 0; i < (*p)->num_matches; i++){
    free((*p)->matches_256[i]);
    free((*p)->matches_512[i]);
    free((*p)->matches_1024[i]);
  }
  free((*p)->matches_256);
  free((*p)->matches_512);
  free((*p)->matches_1024);
  for(i = 0; i < (*p)->num_halos; i++){
    free((*p)->halos_256[i]);
    free((*p)->halos_512[i]);
    free((*p)->halos_1024[i]);
  }
  free((*p)->halos_256);
  free((*p)->halos_512);
  free((*p)->halos_1024);
  free((*p)->ascii_output);
}


static void check_file_harsh(char *path, char *error)
{
  if(check_file(path) != CF_FILE_EXIST){
    printf("[Error! %s]\n", error);
    exit(EXIT_FAILURE);
  }
}
