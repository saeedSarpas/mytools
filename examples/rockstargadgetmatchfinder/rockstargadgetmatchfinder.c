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
#include <unistd.h>
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
#include "./../../loading/simple_loading.h"
#include "./../../math/interval/expspaced.h"


typedef struct _params
{
  char *priHalos;
  char *secHalos;
  char *priSnap;
  char *secSnap;
  int loadMatches;
  char *loadMatchesPath;
  double massOffset;
  double maxDisplacement;
  int initVolResolution;
  int saveMatches;
  char *saveMatchesPath;
  int saveSingleMatches;
  char *saveSingleMatchesDir;
  int numSingleMatches;
  char *priLatestSnap;
  char *secLatestSnap;
  char *outputPath;
} params;


static params* loadconfig(const char*);
static void print_results(halofinder*, halofinder*, params*, matchinghalo*);
static void print_halo_particles(matchinghalo*, halofinder*, halofinder*,
                                 snapshot*, snapshot*, snapshot*, snapshot*,
                                 params*);


int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("[Error] You should specify the path of the configuration file.\n");
    printf("        ./rockstarmatchfinder /path/to/the/configuration/file\n");
    exit(EXIT_FAILURE);
  }

  params *p = loadconfig(argv[1]);

  snapshot *prisnap, *secsnap;

  clock_t _l_s_p_ = start("Loading and sorting primary halos");
  halofinder *pri = load_rockstar_bin(p->priHalos);
  sort_rockstar_halos(pri->halos, pri->header->num_halos,
                      compare_mass);
  done(_l_s_p_);

  clock_t _l_s_s_ = start("Loading and sorting secondary halos");
  halofinder *sec = load_rockstar_bin(p->secHalos);
  sort_rockstar_halos(sec->halos, sec->header->num_halos,
                      compare_mass);
  done(_l_s_s_);

  if(!p->loadMatches){
    clock_t _l_p_g_ = start("Loading primary gadget snapshot");
    prisnap = load_gadget_snap(p->priSnap);
    done(_l_p_g_);

    clock_t _l_s_g_ = start("Loading secondary gadget snapshot");
    secsnap = load_gadget_snap(p->secSnap);
    done(_l_s_g_);

    int griddims[3] = {p->initVolResolution,
                       p->initVolResolution,
                       p->initVolResolution};

    clock_t _f_p_s_ = start("Filling init_volume of primary and secondary halos");
    fill_init_volume(pri, prisnap, griddims);
    fill_init_volume(sec, secsnap, griddims);
    done(_f_p_s_);
  }

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

  snapshot *prilatestsnap, *seclatestsnap;

  if(p->saveSingleMatches){
    clock_t _s_h_p_ = start("Saving halo particles\n");

    if(p->loadMatches){
      clock_t _l_p_g_ = start("\tLoading primary gadget snapshot");
      prisnap = load_gadget_snap(p->priSnap);
      done(_l_p_g_);

      clock_t _l_s_g_ = start("\tLoading secondary gadget snapshot");
      secsnap = load_gadget_snap(p->secSnap);
      done(_l_s_g_);

      clock_t _l_p_l_g_ = start("\tLoading primary latest gadget snapshot");
      prilatestsnap = load_gadget_snap(p->priLatestSnap);
      done(_l_p_l_g_);

      clock_t _l_s_l_g_ = start("\tLoading secondary latest gadget snapshot");
      seclatestsnap = load_gadget_snap(p->secLatestSnap);
      done(_l_s_l_g_);
    }

    print_halo_particles(mh, pri, sec, prisnap, secsnap, prilatestsnap,
                         seclatestsnap, p);

    done(_s_h_p_);
  }

  clock_t _c_u_ = start("Clining up");
  dispose_halofinder(pri);
  dispose_halofinder(sec);

  if(!p->loadMatches){
    dispose_snapshot(prisnap);
    dispose_snapshot(secsnap);
  }
  done(_c_u_);

  return 0;
}


/*
 * Checking if a directory exist
 */
static void check_dir(const char *path, const char *err)
{
  char *copied_path = strdup(path);
  DIR* dir = opendir(copied_path);
  if(dir)
    closedir(dir);
  else {
    printf("[%s]\n", err);
    exit(EXIT_FAILURE);
  }
}


/*
 * Checking if a file exist
 */
static void check_file(const char *path, const char *err)
{
  char *copied_path = strdup(path);
  if(access(copied_path, F_OK) == -1){
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
  p->priHalos = strdup(input);

  input = cfg_getstring(inputs, "sec_halos");
  p->secHalos = strdup(input);

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

  config_setting_t *output = cfg_findsetting(cfg, "output");

  p->saveMatches = cfg_getbool(output, "save_matches");
  const char *saveMatchesPath = cfg_getstring(output, "save_matches_path");
  p->saveMatchesPath = strdup(saveMatchesPath);

  p->saveSingleMatches = cfg_getbool(output, "save_single_matches");
  p->numSingleMatches = cfg_getint(output, "num_single_matches");
  const char *saveSingleMatchesDir = cfg_getstring(output,
                                                   "save_single_matches_dir");
  p->saveSingleMatchesDir = strdup(saveSingleMatchesDir);

  const char *latest_snap = cfg_getstring(output, "pri_latest_snap");
  p->priLatestSnap = strdup(latest_snap);

  latest_snap = cfg_getstring(output, "sec_latest_snap");
  p->secLatestSnap = strdup(latest_snap);

  const char *outputPath = cfg_getstring(output, "output_path");
  p->outputPath = strdup(outputPath);

  // Checking if the necessary files/directories exist
  check_file(p->priHalos, "Wrong pri_halos in configs");
  check_file(p->secHalos, "Wrong sec_halos in configs");

  if(p->loadMatches)
    check_file(p->loadMatchesPath, "Wrong load_matches_path in configs");
  else{
    check_file(p->priSnap, "Wrong pri_snap in configs");
    check_file(p->secSnap, "Wrong sec_snap in configs");
  }

  if(p->saveMatches){
    char *saveMatchesDir = strdup(p->saveMatchesPath);
    check_dir(dirname(saveMatchesDir), "Wrong save_matches_path in configs");
  }

  if(p->saveSingleMatches){
    check_dir(p->saveSingleMatchesDir, "Wrong save_single_matches_dir in configs");
    check_file(p->priSnap, "Wrong pri_snap in configs");
    check_file(p->secSnap, "Wrong sec_snap in configs");
    check_file(p->priLatestSnap, "Wrong pri_latest_snap in configs");
    check_file(p->secLatestSnap, "Wrong sec_latest_snap in configs");
  }

  char *outputPathDir = strdup(outputPath);
  check_dir(dirname(outputPathDir), "Wrong output_path in configs");

  return p;
}


static void print_results(halofinder *pri, halofinder *sec,
                          params *p, matchinghalo *mh)
{
  int i, foundmatches = 0;
  for(i = 0; i < pri->header->num_halos; i++)
    if(mh->matchingids[i] != MATCHINGHALONOTSET) foundmatches++;

  FILE *outputfile = open_file(p->outputPath, "w");

  fprintf(outputfile, "# Primary input: %s\n", p->priHalos);
  fprintf(outputfile, "# Secondary input: %s\n", p->secHalos);
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
      fprintf(outputfile, "%7d %17.15e %7d %17.15e %6.2f\n",
              i, pri->halos[i].m,
              mh->matchingids[i], sec->halos[mh->matchingids[i]].m,
              mh->goodnesses[i]);

  fclose(outputfile);
}


static void print_halo_particles(matchinghalo *mh,
                                 halofinder *pri, halofinder *sec,
                                 snapshot *prisnap, snapshot *secsnap,
                                 snapshot *prilatestsnap, snapshot *seclatestsnap,
                                 params *p)
{
  int i, j, pri_id, sec_id, part_id;
  char fmt[] = "%17.15e\t%17.15e\t%17.15e\n";

  double *edges = expspaced(0, pri->header->num_halos, p->numSingleMatches);

  for(i = 1; i <= p->numSingleMatches; i++){

    pri_id = (int)edges[i];

    if(mh->matchingids[pri_id] == MATCHINGHALONOTSET) continue;

    char name[512];
    sprintf(name, "%s%d.dat", p->saveSingleMatchesDir, pri_id);

    FILE *fp = open_file(name, "w");

    fprintf(fp, "pri_halo_id: %d\n", pri_id);
    fprintf(fp, "pri_halo_mass: %15.13e\n", pri->halos[pri_id].m);
    fprintf(fp, "pri_halo_num_p: %d\n", (int)pri->halos[pri_id].num_p);

    sec_id = mh->matchingids[pri_id];
    fprintf(fp, "sec_halo_id: %d\n", sec_id);
    fprintf(fp, "sec_halo_mass: %15.13e\n", sec->halos[sec_id].m);
    fprintf(fp, "sec_halo_num_p: %d\n", (int)sec->halos[sec_id].num_p);

    fprintf(fp, "goodness: %f\n", mh->goodnesses[pri_id]);

    // Writing primary halos initial particles
    for(j = 0; j < pri->halos[pri_id].num_p; j++){
      part_id = pri->halos[pri_id].particle_ids[j];

      fprintf(fp, fmt, prisnap->particles[part_id].pos[0],
              prisnap->particles[part_id].pos[1],
              prisnap->particles[part_id].pos[2]);
    }

    // Writing secondary halos initial particles
    for(j = 0; j < sec->halos[sec_id].num_p; j++){
      part_id = sec->halos[sec_id].particle_ids[j];

      fprintf(fp, fmt, secsnap->particles[part_id].pos[0],
              secsnap->particles[part_id].pos[1],
              secsnap->particles[part_id].pos[2]);
    }

    // Writing primary halos latest snapshot particles
    for(j = 0; j < pri->halos[pri_id].num_p; j++){
      part_id = pri->halos[pri_id].particle_ids[j];

      fprintf(fp, fmt, prisnap->particles[part_id].pos[0],
              prilatestsnap->particles[part_id].pos[1],
              prilatestsnap->particles[part_id].pos[2]);
    }

    // Writing secondary halos latest snapshot particles
    for(j = 0; j < sec->halos[sec_id].num_p; j++){
      part_id = sec->halos[sec_id].particle_ids[j];

      fprintf(fp, fmt, secsnap->particles[part_id].pos[0],
              seclatestsnap->particles[part_id].pos[1],
              seclatestsnap->particles[part_id].pos[2]);
    }

    fclose(fp);
  }
}
