/*
 * examples/rg_mmatchfinder/mh_history/history_params.c
 *
 * Loading and disposing parameters for history module
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./accretion_history.h"
#include "./../../../vector/vector_get.h"
#include "./../../../vector/vector_push.h"
#include "./../../../time/mytime.h"
#include "./../../../ll/ll.h"
#include "./../../../memory/allocate.h"
#include "./../../../math/fittingfunctions/fityax.h"
#include "./../../../io/open_file.h"

static vector* snapshot_redshifts();

void accretion_history(halofinder ***rockstars,
                       vector **cascades,
                       mh_history_params *p)
{
  // Saving halo history properties to disk
  FILE *ascii = open_file(p->ascii_output, "w");

  fprintf(ascii, "#id_256\talpha_256\tz30_256\tz50_256\tz70_256\t"
          "id_512\talpha_512\tz30_512\tz50_512\tz70_512\t"
          "id_1024\talpha_1024\tz30_1024\tz50_1024\tz70_1024\n");

  int i, j, c_id;
  int id[3];

  double alpha[3], z30[3], z50[3], z70[3];
  double scaled_mass;
  fitresultyax *result = NULL;

  ll **lists = allocate(3, sizeof(*lists));
  llnode *item = NULL;

  vector *redshitfs = snapshot_redshifts();
  vector **scaled_masses = allocate(3, sizeof(*scaled_masses));

  for(c_id = 0;
      c_id < rockstars[0][p->num_halo_files - 1]->header->num_halos;
      c_id++){
    if(cascades[c_id]->logLength != 3)
      continue;

    for(i = 0; i < 3; i++)
      lists[i] = (ll*)vector_get(cascades[c_id], i);

    // We need at least two points for fitting function
    // Here we consider 3 points for having a better fit
    if(lists[0]->len < 3 || lists[1]->len < 3 || lists[2]->len < 3)
      continue;

    for(i = 0; i < 3; i++){
      id[i] = lists[i]->head->ikey;

      scaled_masses[i] = new_vector(24, sizeof(double));

      item = lists[i]->head;

      j = p->num_halo_files - 1;

      while(item){
        scaled_mass = log(rockstars[i][j]->halos[item->ikey].m /
                          rockstars[i][p->num_halo_files - 1]->halos[id[i]].m);
        vector_push(scaled_masses[i],  &scaled_mass);

        item = item->next;
        j--;
      }

      redshitfs->logLength = scaled_masses[i]->logLength;
      result = fityax(redshitfs, scaled_masses[i]);

      alpha[i] = result->slope;

      z30[i] = log(0.30) / (alpha[i]);
      z50[i] = log(0.50) / (alpha[i]);
      z70[i] = log(0.70) / (alpha[i]);

      dispose_vector(&scaled_masses[i]);
    }

    for(i = 0; i < 3; i++){
      fprintf(ascii, "%8d\t%8.6e\t%8.6e\t%8.6e\t%8.6e\t",
              id[i], alpha[i], z30[i], z50[i], z70[i]);
    }
    fprintf(ascii, "\n");
  }

  fclose(ascii);
}


/*
 * Generating a vector of the expansion scale factors of the snapshots
 */
static vector* snapshot_redshifts()
{
  vector *scales = new_vector(24, sizeof(double));

  double a[24] = {1.0, 1.0, 0.947367988916, 0.894735991232, 0.842104990301,
                  0.78947299346, 0.749999992058, 0.736841999079, 0.684209991412,
                  0.63157799597, 0.578946995024, 0.499999996885, 0.44999999634,
                  0.368420997244, 0.333332995824, 0.249999996885, 0.210525998262,
                  0.166666999761, 0.105262998475, 0.083332998916, 0.0526309993666,
                  0.0416659999783, 0.0208329998593, 0.01960784313};
  int i;
  double z;
  for(i = 0; i < 24; i++){
    z = (1.0 / a[i]) - 1.0;
    vector_push(scales, &z);
  }

  return scales;
}
