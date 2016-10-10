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
  char *priHalos;
  char *secHalos;
  char *priSnap;
  char *secSnap;
  double massOffset;
  double minGoodness;
  int initVolResolution;
  char *outputpath;
} params;


static params* loadconfig(const char*);


int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("[Error] You should specify the path of the configuration file.\n");
    printf("        ./rockstarmatchfinder /path/to/the/configuration/file\n");
    exit(EXIT_FAILURE);
  }

  params *p = loadconfig(argv[1]);

  FILE *prihalosfile = open_file(p->priHalos, "rb");
  halofinder *prihalos = load_rockstar_bin(prihalosfile);
  fclose(prihalosfile);

  FILE *sechalosfile = open_file(p->secHalos, "rb");
  halofinder *sechalos = load_rockstar_bin(sechalosfile);
  fclose(sechalosfile);

  FILE *prisnapfile = open_file(p->priSnap, "rb");
  snapshot *prisnap = load_gadget_snap(prisnapfile);
  fclose(prisnapfile);

  FILE *secsnapfile = open_file(p->secSnap, "rb");
  snapshot *secsnap = load_gadget_snap(secsnapfile);
  fclose(secsnapfile);
  printf("%d\n", secsnap->header->npart[1]);


  double boxlength[3] = {prihalos->header->box_size,
                         prihalos->header->box_size,
                         prihalos->header->box_size};
  int griddims[3] = {p->initVolResolution,
                     p->initVolResolution,
                     p->initVolResolution};

  fill_init_volume(prihalos, prisnap, boxlength, griddims);
  fill_init_volume(sechalos, secsnap, boxlength, griddims);

  halomatcher_params hm_p = {.massoffset = p->massOffset,
                             .mingoodness = p->minGoodness};

  matchinghalo *mh = halomatcher(prihalos, sechalos, hm_p);


  int i, foundmatches = 0;
  for(i = 0; i < prihalos->header->num_halos; i++)
    if(mh->matchingids[i] == MATCHINGHALONOTSET) foundmatches++;

  FILE *outputfile = open_file(p->outputpath, "w");
  fprintf(outputfile, "# Primary input: %s\n", p->priHalos);
  fprintf(outputfile, "# # of halos in primary input: %" PRId64 "\n",
          prihalos->header->num_halos);
  fprintf(outputfile, "# Secondary input: %s\n", p->secHalos);
  fprintf(outputfile, "# # of halos in secondary input: %" PRId64 "\n",
          sechalos->header->num_halos);
  fprintf(outputfile, "# # of found matches: %d\n", foundmatches);

  for(i = 0; i < prihalos->header->num_halos; i++)
    if(mh->matchingids[i] != MATCHINGHALONOTSET)
      fprintf(outputfile, "%d %d %f\n", i, mh->matchingids[i], mh->goodnesses[i]);

  dispose_halofinder(prihalos);
  dispose_snapshot(prisnap);
  dispose_halofinder(sechalos);
  dispose_snapshot(secsnap);

  return 0;
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

  p->massOffset = cfg_getdouble(options, "mass_offset");
  p->minGoodness = cfg_getdouble(options, "min_goodness");
  p->initVolResolution = cfg_getint(options, "init_volume_resolution");

  config_setting_t *output = cfg_findsetting(cfg, "output");

  const char *outputpath = cfg_getstring(output, "output_path");
  p->outputpath = strdup(outputpath);

  return p;
}
