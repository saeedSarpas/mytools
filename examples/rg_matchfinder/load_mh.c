/*
 * examples/rg_matchfinder/load_mh.c
 * test_file: examples/rg_matchfinder/load_mh_test.c
 *
 * Loading matching halos output generated by
 * examples/rg_matchfinder/rg_matchfinder
 *
 * param:
 *
 * return:
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "./load_mh.h"
#include "./../../io/open_file.h"
#include "./../../memory/allocate.h"


static void go_to_line(FILE*, int);


avl_node* load_mh(char *path)
{
  FILE *matches_file = open_file(path, "r");

  go_to_line(matches_file, 8);

  avl_node *matches = allocate(1, sizeof(*matches));

  int pri_halo_id;
  float dummy;
  match *match_holder = allocate(1, sizeof(*match_holder));

  int i, num_of_matches = get_num_of_mh(path);
  for(i = 0; i < num_of_matches; i++){
    fscanf(matches_file, " %d %e %d %e %f\n", &pri_halo_id, &dummy,
           &match_holder->matchid, &dummy, &match_holder->goodness);
    matches = avl_insert(matches, pri_halo_id, match_holder, 1, sizeof(match));
  }

  free(match_holder);
  fclose(matches_file);

  return matches;
}


int get_num_of_mh(char *path)
{
  FILE *matches_file = open_file(path, "r");

  go_to_line(matches_file, 7);

  int num_of_matches;
  fscanf(matches_file, "num of found matches: %d\n", &num_of_matches);

  return num_of_matches;
}


static void go_to_line(FILE *file, int line)
{
  int counter;
  while(counter < line){
    char c = fgetc(file);
    if(c == '\n')
      counter++;
  }
}
