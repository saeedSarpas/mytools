/*
 * examples/rg_matchfinder/rg_matchfinder.c
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
#include "./../../simulation/load_snap_from.h"
#include "./../../simulation/save_snap_to.h"
#include "./../../simulation/check_if_saved_exist.h"
#include "./../../simulation/gadget/load_gadget_snap.h"
#include "./../../halomatcher/matchinghalo.h"
#include "./../../halomatcher/halomatcher.h"
#include "./../../loading/simple_loading.h"
#include "./../../math/interval/expspaced.h"


#define INTERNALMATCHINGTRUE 1
#define INTERNALMATCHINGFALSE 0


typedef struct _params
{
  char *priHalosAddr;
  char *secHalosAddr;
  char *priICAddr;
  char *secICAddr;
  int internalMatching;
  int loadBinMatches;
  char *binMatchesAddr;
  double massOffset;
  double maxDisplacement;
  int initVolResolution;
  int saveBinMatches;
  int saveSingleMatches;
  char *singleMatchesDir;
  int numSingleMatches;
  char *priSnapAddr;
  char *secSnapAddr;
  char *asciiOutputPath;
} params;


static params* loadconfig(const char*);
static snapshot* safe_snapshot_load(char*);
static void print_results(halofinder*, halofinder*, params*, matchinghalo*);
static void print_halo_particles(matchinghalo*, halofinder*, halofinder*,
                                 snapshot*, snapshot*,
                                 snapshot*, snapshot*, params*);


int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("[Error] You should specify the path of the configuration file.\n");
    printf("        ./rockstarmatchfinder /path/to/the/configuration/file\n");
    exit(EXIT_FAILURE);
  }

  params *p = loadconfig(argv[1]);

  snapshot *pri_ic, *sec_ic;

  clock_t _l_s_p_ = start("Loading and sorting primary halos");
  halofinder *pri_msorted = load_rockstar_bin(p->priHalosAddr);
  sort_rockstar_halos(pri_msorted->halos, pri_msorted->header->num_halos,
                      compare_mass);
  done(_l_s_p_);

  clock_t _l_s_s_ = start("Loading and sorting secondary halos");
  halofinder *sec_msorted = load_rockstar_bin(p->secHalosAddr);
  sort_rockstar_halos(sec_msorted->halos, sec_msorted->header->num_halos,
                      compare_mass);
  done(_l_s_s_);

  if(!p->loadBinMatches){
    clock_t _l_p_g_ = start("Loading primary gadget snapshot");
    pri_ic = safe_snapshot_load(p->priICAddr);
    done(_l_p_g_);

    if(p->internalMatching == INTERNALMATCHINGTRUE){
      clock_t _i_m_ = start("Internal Matching (no need to load the snapshot)");
      sec_ic = pri_ic;
      done(_i_m_);
    } else {
      clock_t _l_s_g_ = start("Loading secondary gadget snapshot");
      sec_ic = safe_snapshot_load(p->secICAddr);
      done(_l_s_g_);
    }

    int griddims[3] = {p->initVolResolution,
                       p->initVolResolution,
                       p->initVolResolution};

    clock_t _f_p_s_ = start("Filling init_volume of primary and secondary halos");
    fill_init_volume(pri_msorted, pri_ic, griddims);
    fill_init_volume(sec_msorted, sec_ic, griddims);
    done(_f_p_s_);
  }

  halomatcher_params hm_p = {.massOffset = p->massOffset,
                             .maxDisplacement = p->maxDisplacement,
                             .loadMatches = p->loadBinMatches,
                             .loadMatchesPath = p->binMatchesAddr,
                             .saveMatches = p->saveBinMatches,
                             .saveMatchesPath = p->binMatchesAddr};

  clock_t _m_h_ = start("Finding matching halos\n");
  matchinghalo *mh = halomatcher(pri_msorted, sec_msorted, hm_p);
  done(_m_h_);

  clock_t _w_r_ = start("Writing results");
  print_results(pri_msorted, sec_msorted, p, mh);
  done(_w_r_);

  snapshot *pri_snap, *sec_snap;

  if(p->saveSingleMatches){
    clock_t _s_h_p_ = start("Saving halo particles\n");

    if(p->loadBinMatches){
      clock_t _l_p_g_ = start("\tLoading primary gadget snapshot");
      pri_ic = safe_snapshot_load(p->priICAddr);
      done(_l_p_g_);

      if(p->internalMatching == INTERNALMATCHINGTRUE){
        clock_t _i_m_ = start("\tInternal Matching (no need to load the sec_ic)");
        sec_ic = pri_ic;
        done(_i_m_);
      } else {
        clock_t _l_s_g_ = start("\tLoading secondary gadget snapshot");
        sec_ic = safe_snapshot_load(p->secICAddr);
        done(_l_s_g_);
      }
    }

    clock_t _l_p_l_g_ = start("\tLoading primary latest gadget snapshot");
    pri_snap = safe_snapshot_load(p->priSnapAddr);
    done(_l_p_l_g_);

    clock_t _l_s_l_g_ = start("\tLoading secondary latest gadget snapshot");
    sec_snap = safe_snapshot_load(p->secSnapAddr);
    done(_l_s_l_g_);

    /* The already loaded pri/sec halos are sorted by their masses which is not */
    /* helpful here, so we sort them again */
    halofinder *pri = load_rockstar_bin(p->priHalosAddr);
    halofinder *sec = load_rockstar_bin(p->secHalosAddr);
    print_halo_particles(mh, pri, sec, pri_ic, sec_ic, pri_snap, sec_snap, p);
    dispose_halofinder(&pri);
    dispose_halofinder(&sec);

    dispose_snapshot(&pri_snap);
    dispose_snapshot(&sec_snap);

    done(_s_h_p_);
  }

  clock_t _c_u_ = start("Cleaning up");
  dispose_halofinder(&pri_msorted);
  dispose_halofinder(&sec_msorted);

  if(!p->loadBinMatches || p->saveSingleMatches){
    dispose_snapshot(&pri_ic);
    if(p->internalMatching != INTERNALMATCHINGTRUE)
      dispose_snapshot(&sec_ic);
  }

  done(_c_u_);

  return 0;
}


/*
 * Load gadget snapshot
 * If the saved version exist, this doesn't convert the gadget snapshot to the
 * generic snapshots
 */
static snapshot* safe_snapshot_load(char *path)
{
  if(check_if_saved_exist(path)){
    printf(" [Saved version of the snapshot is already exist]");
    fflush(stdout);
    return load_snap_from(saved_snap_path(path));
  }
  else{
    snapshot *snap = load_gadget_snap(path);
    save_snap_to(saved_snap_path(path), snap);
    return snap;
  }
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

 * param: path path to the configuration file
 */
static params* loadconfig(const char *path)
{
  params *p = allocate(1, sizeof(params));

  config_t *cfg = new_cfg();
  cfg_loadfile(cfg, path);

  config_setting_t *inputs = cfg_findsetting(cfg, "inputs");

  const char *input = cfg_getstring(inputs, "pri_halos_addr");
  p->priHalosAddr = strdup(input);

  input = cfg_getstring(inputs, "sec_halos_addr");
  p->secHalosAddr = strdup(input);

  input = cfg_getstring(inputs, "pri_ic_addr");
  p->priICAddr = strdup(input);

  input = cfg_getstring(inputs, "sec_ic_addr");
  p->secICAddr = strdup(input);

  if(strcmp(p->priICAddr, p->secICAddr) == 0)
    p->internalMatching = INTERNALMATCHINGTRUE;
  else
    p->internalMatching = INTERNALMATCHINGFALSE;


  config_setting_t *options = cfg_findsetting(cfg, "options");

  p->loadBinMatches = cfg_getbool(options, "load_bin_matches");
  const char *binMatchesAddr = cfg_getstring(options, "bin_matches_addr");
  p->binMatchesAddr = strdup(binMatchesAddr);

  p->massOffset = cfg_getdouble(options, "mass_offset");
  p->initVolResolution = cfg_getint(options, "init_volume_resolution");
  p->maxDisplacement = cfg_getdouble(options, "max_halo_displacement");

  config_setting_t *output = cfg_findsetting(cfg, "output");

  p->saveBinMatches = cfg_getbool(output, "save_bin_matches");

  p->saveSingleMatches = cfg_getbool(output, "save_single_matches");
  p->numSingleMatches = cfg_getint(output, "num_of_single_matches");
  const char *singleMatchesDir = cfg_getstring(output, "single_matches_dir");
  p->singleMatchesDir = strdup(singleMatchesDir);

  const char *latest_snap = cfg_getstring(output, "pri_snap_addr");
  p->priSnapAddr = strdup(latest_snap);

  latest_snap = cfg_getstring(output, "sec_snap_addr");
  p->secSnapAddr = strdup(latest_snap);

  const char *asciiOutputPath = cfg_getstring(output, "ascii_output_path");
  p->asciiOutputPath = strdup(asciiOutputPath);

  // Checking if the necessary files/directories exist
  check_file(p->priHalosAddr, "Wrong pri_halos in configs");
  check_file(p->secHalosAddr, "Wrong sec_halos in configs");

  if(p->loadBinMatches)
    check_file(p->binMatchesAddr, "Wrong load_matches_path in configs");
  else{
    check_file(p->priICAddr, "Wrong pri_snap in configs");
    check_file(p->secICAddr, "Wrong sec_snap in configs");
  }

  if(p->saveBinMatches){
    char *saveBinMatchesDir = strdup(p->binMatchesAddr);
    check_dir(dirname(saveBinMatchesDir), "Wrong save_matches_path in configs");
  }

  if(p->saveSingleMatches){
    check_dir(p->singleMatchesDir, "Wrong save_single_matches_dir in configs");
    check_file(p->priICAddr, "Wrong pri_snap in configs");
    check_file(p->secICAddr, "Wrong sec_snap in configs");
    check_file(p->priSnapAddr, "Wrong pri_latest_snap in configs");
    check_file(p->secSnapAddr, "Wrong sec_latest_snap in configs");
  }

  char *asciiOutputPathDir = strdup(asciiOutputPath);
  check_dir(dirname(asciiOutputPathDir), "Wrong output_path in configs");

  return p;
}


static void print_results(halofinder *pri, halofinder *sec,
                          params *p, matchinghalo *mh)
{
  int i, foundmatches = 0;
  for(i = 0; i < pri->header->num_halos; i++)
    if(mh->matchingids[i] != MATCHINGHALONOTSET) foundmatches++;

  FILE *outputfile = open_file(p->asciiOutputPath, "w");

  fprintf(outputfile, "Primary input: %s\n", p->priHalosAddr);
  fprintf(outputfile, "Secondary input: %s\n", p->secHalosAddr);
  fprintf(outputfile, "Mass offset: %f\n", p->massOffset);
  fprintf(outputfile, "Maximum halo displacement: %f\n", p->maxDisplacement);
  fprintf(outputfile, "Initial volume grid: %d\n", p->initVolResolution);
  fprintf(outputfile, "num of halos in primary input: %" PRId64 "\n",
          pri->header->num_halos);
  fprintf(outputfile, "num of halos in secondary input: %" PRId64 "\n",
          sec->header->num_halos);
  fprintf(outputfile, "num of found matches: %d\n", foundmatches);

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
                                 snapshot *pri_ic, snapshot *sec_ic,
                                 snapshot *pri_snap, snapshot *sec_snap,
                                 params *p)
{
  int i, j, pri_id, sec_id, part_id;
  char fmt[] = "%17.15e\t%17.15e\t%17.15e\n";

  double *edges = expspaced(0, pri->header->num_halos, p->numSingleMatches);

  for(i = 1; i <= p->numSingleMatches; i++){
    pri_id = (int)edges[i];

    if(mh->matchingids[pri_id] == MATCHINGHALONOTSET) continue;
    if(pri->halos[pri_id].id == HALONOTSET) continue;

    sec_id = mh->matchingids[pri_id];
    if(sec->halos[sec_id].id == HALONOTSET) continue;

    char name[512];
    sprintf(name, "%s%d.dat", p->singleMatchesDir, pri_id);

    FILE *fp = open_file(name, "w");

    fprintf(fp, "pri_halo_id: %d\n", pri_id);
    fprintf(fp, "pri_halo_mass: %15.13e\n", pri->halos[pri_id].m);
    fprintf(fp, "pri_halo_num_p: %d\n", (int)pri->halos[pri_id].num_p);

    fprintf(fp, "sec_halo_id: %d\n", sec_id);
    fprintf(fp, "sec_halo_mass: %15.13e\n", sec->halos[sec_id].m);
    fprintf(fp, "sec_halo_num_p: %d\n", (int)sec->halos[sec_id].num_p);

    fprintf(fp, "goodness: %f\n", mh->goodnesses[pri_id]);

    // Writing primary halos initial particles
    for(j = 0; j < pri->halos[pri_id].num_p; j++){
      part_id = pri->halos[pri_id].particle_ids[j];

      fprintf(fp, fmt, pri_ic->particles[part_id].pos[0],
              pri_ic->particles[part_id].pos[1],
              pri_ic->particles[part_id].pos[2]);
    }

    // Writing secondary halos initial particles
    for(j = 0; j < sec->halos[sec_id].num_p; j++){
      part_id = sec->halos[sec_id].particle_ids[j];

      fprintf(fp, fmt, sec_ic->particles[part_id].pos[0],
              sec_ic->particles[part_id].pos[1],
              sec_ic->particles[part_id].pos[2]);
    }

    // Writing primary halos latest snapshot particles
    for(j = 0; j < pri->halos[pri_id].num_p; j++){
      part_id = pri->halos[pri_id].particle_ids[j];

      fprintf(fp, fmt, pri_snap->particles[part_id].pos[0],
              pri_snap->particles[part_id].pos[1],
              pri_snap->particles[part_id].pos[2]);
    }

    // Writing secondary halos latest snapshot particles
    for(j = 0; j < sec->halos[sec_id].num_p; j++){
      part_id = sec->halos[sec_id].particle_ids[j];

      fprintf(fp, fmt, sec_snap->particles[part_id].pos[0],
              sec_snap->particles[part_id].pos[1],
              sec_snap->particles[part_id].pos[2]);
    }

    fclose(fp);
  }
}
