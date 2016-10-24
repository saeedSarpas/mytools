/*
 * examples/rockstargadgetmatchfinder/rockstargadgetmatchfinder.c
 *
 * Finding the matching halos between two rockstar halo lists
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include "./../../time/mytime.h"
#include "./../../configfile/mylibconfig.h"
#include "./../../memory/allocate.h"
#include "./../../io/open_file.h"
#include "./../../halofinder/halofinder.h"
#include "./../../halofinder/rockstar/load_rockstar_bin.h"
#include "./../../halofinder/rockstar/sort_rockstar_halos.h"
#include "./../../halofinder/fill_init_volume.h"
#include "./../../simulation/snapshot.h"
#include "./../../simulation/gadget/load_gadget_snap.h"
#include "./../../halomatcher/matchinghalo.h"
#include "./../../halomatcher/halomatcher.h"


typedef struct _params
{
  char *priFileAddr;
  char *secFileAddr;
  char *priSnap;
  char *secSnap;
  int loadMatches;
  char *loadMatchesPath;
  double massOffset;
  double maxDisplacement;
  int initVolResolution;
  int saveMatches;
  char *saveMatchesPath;
  char *outputPath;
} params;


static params* loadconfig(const char*);
static void print_results(halofinder*, halofinder*, params*, matchinghalo*);


int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("[Error] You should specify the path of the configuration file.\n");
    printf("        ./rockstarmatchfinder /path/to/the/configuration/file\n");
    exit(EXIT_FAILURE);
  }

  params *p = loadconfig(argv[1]);

  clock_t _l_s_p_ = start("Loading and sorting primary halos");
  halofinder *pri = load_rockstar_bin(p->priFileAddr);
  sort_rockstar_halos(pri->halos, pri->header->num_halos,
                      compare_mass);
  done(_l_s_p_);

  clock_t _l_s_s_ = start("Loading and sorting secondary halos");
  halofinder *sec = load_rockstar_bin(p->secFileAddr);
  sort_rockstar_halos(sec->halos, sec->header->num_halos,
                      compare_mass);
  done(_l_s_s_);

  clock_t _l_p_g_ = start("Loading primary gadget snapshot");
  snapshot *prisnap = load_gadget_snap(p->priSnap);
  done(_l_p_g_);

  clock_t _l_s_g_ = start("Loading secondary gadget snapshot");
  snapshot *secsnap = load_gadget_snap(p->secSnap);
  done(_l_s_g_);

  double boxlength[3] = {pri->header->box_size[0],
                         pri->header->box_size[1],
                         pri->header->box_size[2]};

  int griddims[3] = {p->initVolResolution,
                     p->initVolResolution,
                     p->initVolResolution};

  clock_t _f_p_s_ = start("Filling init_volume of primary and secondary halos");
  fill_init_volume(pri, prisnap, boxlength, griddims);
  fill_init_volume(sec, secsnap, boxlength, griddims);
  done(_f_p_s_);

  halomatcher_params hm_p = {.massOffset = p->massOffset,
                             .maxDisplacement = p->maxDisplacement,
                             .loadMatches = p->loadMatches,
                             .loadMatchesPath = p->loadMatchesPath,
                             .saveMatches = p->saveMatches,
                             .saveMatchesPath = p->saveMatchesPath};

  clock_t _m_h_ = start("Finding matching halos\n");
  matchinghalo *mh = halomatcher(pri, sec, hm_p);
  done(_m_h_);

  clock_t _w_r_ = start("Writing results");
  print_results(pri, sec, p, mh);
  done(_w_r_);

  dispose_halofinder(pri);
  dispose_snapshot(prisnap);
  dispose_halofinder(sec);
  dispose_snapshot(secsnap);

  return 0;
}


/*
 * Checking if the directory exist
 */
static void check_dir(const char *path, const char *err)
{
  DIR* dir = opendir(path);
  if(dir)
    closedir(dir);
  else {
    printf("[%s]\n", err);
    exit(EXIT_FAILURE);
  }
}


/*
 * Loading configuration file into a param structure
 *
 * param: path path to the configuration file
 */
static params* loadconfig(const char *path)
{
  params *p = allocate(1, sizeof(params));

  config_t *cfg = new_cfg();
  cfg_loadfile(cfg, path);

  config_setting_t *inputs = cfg_findsetting(cfg, "inputs");

  const char *input = cfg_getstring(inputs, "pri_halos");
  p->priFileAddr = strdup(input);

  input = cfg_getstring(inputs, "sec_halos");
  p->secFileAddr = strdup(input);

  input = cfg_getstring(inputs, "pri_snap");
  p->priSnap = strdup(input);

  input = cfg_getstring(inputs, "sec_snap");
  p->secSnap = strdup(input);

  config_setting_t *options = cfg_findsetting(cfg, "options");

  p->loadMatches = cfg_getbool(options, "load_matches");
  const char *loadMatchesPath = cfg_getstring(options, "load_matches_path");
  p->loadMatchesPath = strdup(loadMatchesPath);

  p->massOffset = cfg_getdouble(options, "mass_offset");
  p->initVolResolution = cfg_getint(options, "init_volume_resolution");
  p->maxDisplacement = cfg_getdouble(options, "max_halo_displacement");

  p->saveMatches = cfg_getbool(options, "save_matches");
  const char *saveMatchesPath = cfg_getstring(options, "save_matches_path");
  p->saveMatchesPath = strdup(saveMatchesPath);

  config_setting_t *output = cfg_findsetting(cfg, "output");

  const char *outputPath = cfg_getstring(output, "output_path");
  p->outputPath = strdup(outputPath);

  if(p->loadMatches)
    check_dir(dirname(p->loadMatchesPath), "Wrong load_matches_path in configs");

  if(p->saveMatches)
    check_dir(dirname(p->saveMatchesPath), "Wrong save_matches_path in configs");

  check_dir(dirname(p->outputPath), "Wrong output_path in configs");

  return p;
}


static void print_results(halofinder *pri, halofinder *sec,
                          params *p, matchinghalo *mh)
{
  int i, foundmatches = 0;
  for(i = 0; i < pri->header->num_halos; i++)
    if(mh->matchingids[i] != MATCHINGHALONOTSET) foundmatches++;

  FILE *outputfile = open_file(p->outputPath, "w");

  fprintf(outputfile, "# Primary input: %s\n", p->priFileAddr);
  fprintf(outputfile, "# Secondary input: %s\n", p->secFileAddr);
  fprintf(outputfile, "# Mass offset: %f\n", p->massOffset);
  fprintf(outputfile, "# Maximum halo displacement: %f\n", p->maxDisplacement);
  fprintf(outputfile, "# Initial volume grid: %d\n", p->initVolResolution);
  fprintf(outputfile, "# # of halos in primary input: %" PRId64 "\n",
          pri->header->num_halos);
  fprintf(outputfile, "# # of halos in secondary input: %" PRId64 "\n",
          sec->header->num_halos);
  fprintf(outputfile, "# # of found matches: %d\n", foundmatches);

  for(i = 0; i < pri->header->num_halos; i++)
    if(mh->matchingids[i] != MATCHINGHALONOTSET)
      fprintf(outputfile, "%d %f %d %f %f\n",
              i, pri->halos[i].m,
              mh->matchingids[i], sec->halos[mh->matchingids[i]].m,
              mh->goodnesses[i]);

  fclose(outputfile);
}
