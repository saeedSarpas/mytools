/*
 * halomatcher/load_save_matches_test.c
 * test_for: halomatcher/load_save_matches.c
 *
 * author: Saeed Sarpas
 */


#include <cgreen/cgreen.h>
#include <stdio.h>
#include "load_save_matches.h"
#include "halomatcher.h"
#include "./../vector/vector_get.h"
#include "./../vector/vector_push.h"
#include "./../io/open_file.h"
#include "./../io/write_to.h"
#include "./../memory/allocate.h"


Describe(load_save_matches);


#define MATCHES_FILE_ADDR "./matches.file"
#define N_PRI_HALOS 11
#define N_SEC_HALOS 13
#define MATCHID 1234
#define GOODNESS 12.34


static int *nprimatches, *nsecmatches;
static int *pristarts, *secstarts;


BeforeEach(load_save_matches)
{
  FILE *file = open_file(MATCHES_FILE_ADDR, "wb");

  matchesfileheader header;
  header.nprihalos = N_PRI_HALOS;
  header.nsechalos = N_SEC_HALOS;

  nprimatches = allocate(header.nprihalos, sizeof(int));
  pristarts = allocate(header.nprihalos, sizeof(int));

  nsecmatches = allocate(header.nsechalos, sizeof(int));
  secstarts = allocate(header.nsechalos, sizeof(int));

  int i, start = 0;
  for(i = 0; i < header.nprihalos; i++){
    pristarts[i] = start;
    nprimatches[i] = i;
    start += i;
  }

  start = 0;
  for(i = 0; i < header.nsechalos; i++){
    secstarts[i] = start;
    nsecmatches[i] = i;
    start += i;
  }

  write_to(file, 1, sizeof(matchesfileheader), &header);

  write_to(file, header.nprihalos, sizeof(int), nprimatches);
  write_to(file, header.nprihalos, sizeof(int), pristarts);
  write_to(file, header.nsechalos, sizeof(int), nsecmatches);
  write_to(file, header.nsechalos, sizeof(int), secstarts);

  int j;
  match dummy_match = {.matchid = MATCHID, .goodness = GOODNESS};
  for(i = 0; i < header.nprihalos; i++){
    vector *dummy_vector = new_vector(i+1, sizeof(match));
    for(j = 0; j <= i; j++)
      vector_push(dummy_vector, &dummy_match);
    write_to(file, i, sizeof(match), dummy_vector->elems);
    dispose_vector(&dummy_vector);
  }

  for(i = 0; i < header.nsechalos; i++){
    vector *dummy_vector = new_vector(i+1, sizeof(match));
    for(j = 0; j <= i; j++)
      vector_push(dummy_vector, &dummy_match);
    write_to(file, i, sizeof(match), dummy_vector->elems);
    dispose_vector(&dummy_vector);
  }

  fclose(file);
}


AfterEach(load_save_matches)
{
  free(nprimatches);
  free(pristarts);
  free(nsecmatches);
  free(secstarts);

  remove(MATCHES_FILE_ADDR);
}


Ensure(load_save_matches, loads_matches_from_disk_properly)
{
  vector **primatches = allocate(N_PRI_HALOS, sizeof(vector*));
  vector **secmatches = allocate(N_SEC_HALOS, sizeof(vector*));

  load_matches(MATCHES_FILE_ADDR, primatches, secmatches);

  int i, j;
  match *dummy_match = allocate(1, sizeof(*dummy_match));
  for(i = 0; i < N_PRI_HALOS; i++)
    for(j = 0; j < i; j++){
      dummy_match = vector_get(primatches[i], j);
      assert_that(dummy_match->matchid, is_equal_to(MATCHID));
      assert_that_double(dummy_match->goodness, is_equal_to_double(GOODNESS));
    }

  for(i = 0; i < N_PRI_HALOS; i++)
    free(primatches[i]);

  for(i = 0; i < N_SEC_HALOS; i++)
    free(secmatches[i]);

  free(primatches);
  free(secmatches);
}


Ensure(load_save_matches, saves_matches_to_disk_properly)
{
  vector **primatches = allocate(N_PRI_HALOS, sizeof(vector*));
  vector **secmatches = allocate(N_SEC_HALOS, sizeof(vector*));

  int i, j;

  match dummy_match = {.matchid = MATCHID, .goodness = GOODNESS};
  for(i = 0; i < N_PRI_HALOS; i++){
    primatches[i] = new_vector(i, sizeof(match));
    for(j = 0; j < i; j++)
      vector_push(primatches[i], &dummy_match);
  }

  for(i = 0; i < N_SEC_HALOS; i++){
    secmatches[i] = new_vector(i, sizeof(match));
    for(j = 0; j < i; j++)
      vector_push(secmatches[i], &dummy_match);
  }

  write_matches(MATCHES_FILE_ADDR, primatches, secmatches, N_PRI_HALOS,
                N_SEC_HALOS);

  vector **primatches_r = allocate(N_PRI_HALOS, sizeof(vector*));
  vector **secmatches_r = allocate(N_SEC_HALOS, sizeof(vector*));

  load_matches(MATCHES_FILE_ADDR, primatches_r, secmatches_r);

  match *thematch = allocate(1, sizeof(*thematch));

  for(i = 0; i < N_PRI_HALOS; i++)
    for(j = 0; j < i; j++){
      thematch = vector_get(primatches_r[i], j);
      assert_that(thematch->matchid, is_equal_to(MATCHID));
    }

  free(primatches);
  free(secmatches);
  free(primatches_r);
  free(secmatches_r);

  return;
}


Ensure(load_save_matches, can_handle_null_in_pri_or_secmatches)
{
  vector **primatches = allocate(N_PRI_HALOS, sizeof(vector*));
  vector **secmatches = allocate(N_SEC_HALOS, sizeof(vector*));

  int i, j;

  match dummy_match = {.matchid = MATCHID, .goodness = GOODNESS};
  for(i = 0; i < N_PRI_HALOS; i++){
    if(i % 2 == 0){
      primatches[i] = NULL;
      continue;
    }
    else{
      primatches[i] = new_vector(i, sizeof(match));
      for(j = 0; j < i; j++)
        vector_push(primatches[i], &dummy_match);
    }
  }

  for(i = 0; i < N_SEC_HALOS; i++){
    if(i % 2 == 0){
      secmatches[i] = new_vector(i, sizeof(match));
      for(j = 0; j < i; j++)
        vector_push(secmatches[i], &dummy_match);
    }
    else{
      secmatches[i] = NULL;
      continue;
    }
  }

  write_matches(MATCHES_FILE_ADDR, primatches, secmatches, N_PRI_HALOS,
                N_SEC_HALOS);

  match *thematch = allocate(1, sizeof(*thematch));

  vector **primatches_r = allocate(N_PRI_HALOS, sizeof(vector*));
  vector **secmatches_r = allocate(N_SEC_HALOS, sizeof(vector*));

  load_matches(MATCHES_FILE_ADDR, primatches_r, secmatches_r);

  for(i = 0; i < N_PRI_HALOS; i++)
    if(i % 2 == 0)
      assert_that(primatches_r[i]->logLength, is_equal_to(0));
    else{
      for(j = 0; j < i; j++){
        thematch = vector_get(primatches_r[i], j);
        assert_that(thematch->matchid, is_equal_to(MATCHID));
      }
    }

  for(i = 0; i < N_SEC_HALOS; i++)
    if(i % 2 == 0){
      for(j = 0; j < i; j++){
        thematch = vector_get(secmatches_r[i], j);
        assert_that(thematch->matchid, is_equal_to(MATCHID));
      }
    }
    else
      assert_that(secmatches_r[i]->logLength, is_equal_to(0));

  free(primatches);
  free(secmatches);
  free(primatches_r);
  free(secmatches_r);

  return;
}
