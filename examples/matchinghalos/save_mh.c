/*
 * examples/matchinghalos/save_mh.c
 *
 * Saving matching halos to disk
 *
 * Parameters
 * path: path to the matching halos file
 *
 * return: an avl tree contains matching halos
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include "./save_mh.h"
#include "./../../io/open_file.h"


void save_mh(halofinder *pri, halofinder *sec, matchfinder_params *p,
             matchinghalo *mh)
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
  fprintf(outputfile,
          "#primary_halo primary_halo_mass secondary_halo "
          "secondary_halo_mass goodness\n");

  for(i = 0; i < pri->header->num_halos; i++)
    if(mh->matchingids[i] != MATCHINGHALONOTSET)
      fprintf(outputfile, "%7d %17.15e %7d %17.15e %6.2f\n",
              i, pri->halos[i].m,
              mh->matchingids[i], sec->halos[mh->matchingids[i]].m,
              mh->goodnesses[i]);

  fclose(outputfile);
}
