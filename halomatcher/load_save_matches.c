/*
 * halomatcher/load_save_matches.c
 * test_file: halomatcher/load_save_matches_test.c
 *
 * Loading and saving matches from/to disk
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include "load_save_matches.h"
#include "singlehalo_matcher.h"
#include "./../io/open_file.h"
#include "./../io/read_from.h"
#include "./../io/write_to.h"
#include "./../memory/allocate.h"
#include "./../loading/simple_loading.h"


/*
 * loading found matches from disk
 *
 * param: path path to the matches file
 * param: primatches, secmatches pointer to the array of matches vector
 *
 */
void load_matches(char *path, vector **primatches, vector **secmatches)
{
  FILE *file = open_file(path, "rb");

  matchesfileheader header;
  read_from(file, 1, sizeof(matchesfileheader), &header);

  int *nprimatches = allocate(header.nprihalos, sizeof(int));
  int *pristarts = allocate(header.nprihalos, sizeof(int));

  int *nsecmatches = allocate(header.nsechalos, sizeof(int));
  int *secstarts = allocate(header.nsechalos, sizeof(int));

  read_from(file, header.nprihalos, sizeof(int), nprimatches);
  read_from(file, header.nprihalos, sizeof(int), pristarts);
  read_from(file, header.nsechalos, sizeof(int), nsecmatches);
  read_from(file, header.nsechalos, sizeof(int), secstarts);

  int i, progress = -1;
  int ntot_halos = header.nprihalos + header.nsechalos - 1;

  for(i = 0; i < header.nprihalos; i++){
    primatches[i] = new_vector(nprimatches[i] + 1, sizeof(match));
    read_from(file, nprimatches[i], sizeof(match), primatches[i]->elems);
    primatches[i]->logLength = nprimatches[i];
    progress = simple_loading(progress, i, ntot_halos);
  }

  for(i = 0; i < header.nsechalos; i++){
    secmatches[i] = new_vector(nsecmatches[i] + 1, sizeof(match));
    read_from(file, nsecmatches[i], sizeof(match), secmatches[i]->elems);
    secmatches[i]->logLength = nsecmatches[i];
    progress = simple_loading(progress, i + header.nprihalos, ntot_halos);
  }

  free(nprimatches);
  free(pristarts);
  free(nsecmatches);
  free(secstarts);

  fclose(file);

  return;
}


/*
 * saving found matches on disk
 *
 * param: path path to the file on disk
 * param: primatches, secmatches array of vectors of found matches
 * param: priheader, secheader headers of the primary and secondary list of halos
 *
 */
void write_matches(char *path, vector **primatches, vector **secmatches,
                   int nprihalos, int nsechalos)
{
  matchesfileheader header;
  header.nprihalos = nprihalos;
  header.nsechalos = nsechalos;

  int *nprimatches = allocate(header.nprihalos, sizeof(int));
  int *pristarts = allocate(header.nprihalos, sizeof(int));

  int *nsecmatches = allocate(header.nsechalos, sizeof(int));
  int *secstarts = allocate(header.nsechalos, sizeof(int));

  int i, nmatches = 0;
  for(i = 0; i < header.nprihalos; i++){
    pristarts[i] = nmatches;
    nprimatches[i] = primatches[i] != NULL ? primatches[i]->logLength : 0;
    nmatches += nprimatches[i];
  }

  nmatches = 0;
  for(i = 0; i < header.nsechalos; i++){
    secstarts[i] = nmatches;
    nsecmatches[i] = secmatches[i] != NULL ? secmatches[i]->logLength : 0;
    nmatches += nsecmatches[i];
  }

  FILE *file = open_file(path, "wb");
  write_to(file, 1, sizeof(header), &header);

  write_to(file, header.nprihalos, sizeof(int), nprimatches);
  write_to(file, header.nprihalos, sizeof(int), pristarts);
  write_to(file, header.nsechalos, sizeof(int), nsecmatches);
  write_to(file, header.nsechalos, sizeof(int), secstarts);

  for(i = 0; i < header.nprihalos; i++)
    if(primatches[i] != NULL)
      write_to(file, nprimatches[i], sizeof(match), primatches[i]->elems);

  for(i = 0; i < header.nsechalos; i++)
    if(secmatches[i] != NULL)
      write_to(file, nsecmatches[i], sizeof(match), secmatches[i]->elems);

  fclose(file);

  free(nprimatches);
  free(pristarts);
  free(nsecmatches);
  free(secstarts);

  return;
}
