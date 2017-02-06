/*
 * examples/rg_mmatchfinder/mh_history/mh_history.c
 *
 * Analyzing the changes of matching halo properties along the history of
 * different runs
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./history_config.h"
#include "./accretion_history.h"
#include "./../triplecascade/triplecascade.h"
#include "./../../matchinghalos/load_mh.h"
#include "./../../../memory/allocate.h"
#include "./../../../ll/ll.h"
#include "./../../../ll/ll_traverse.h"
#include "./../../../vector/vector.h"
#include "./../../../vector/vector_get.h"
#include "./../../../vector/vector_push.h"
#include "./../../../avltree/avltree.h"
#include "./../../../avltree/avl_find.h"
#include "./../../../time/mytime.h"
#include "./../../../halofinder/rockstar/load_rockstar_bin.h"
#include "./../../../loading/simple_loading.h"


int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("[Error] You should specify path to the configuration file.\n");
    printf("        ./triplematches /path/to/the/configuration/file\n");
    exit(EXIT_FAILURE);
  }

  mh_history_params *p = load_params(argv[1]);

  // Loading Cross Runs Matching Halos
  clock_t _l_m_h_ = start("Loading cross runs matching halos");
  avltree *matches_512_256 = load_mh(p->matches_512_256);
  avltree *matches_1024_512 = load_mh(p->matches_1024_512);
  done(_l_m_h_);

  // Loading Internal Matching halos
  clock_t _l_i_m_ = start("Loading internal matches");
  avltree **matches_256 = allocate(p->num_match_files, sizeof(avltree*));
  avltree **matches_512 = allocate(p->num_match_files, sizeof(avltree*));
  avltree **matches_1024 = allocate(p->num_match_files, sizeof(avltree*));

  int i;
  for(i = 0; i < p->num_match_files; i++){
    matches_256[i] = load_mh(p->matches_256[i]);
    matches_512[i] = load_mh(p->matches_512[i]);
    matches_1024[i] = load_mh(p->matches_1024[i]);
  }
  done(_l_i_m_);

  // Loading halo files
  clock_t _l_h_ = start("Loading halo files\n");
  halofinder **rockstar[3];

  for(i = 0; i < 3; i++)
    rockstar[i] = allocate(p->num_halo_files, sizeof(*rockstar[i]));

  int progress = -1;
  for(i = 0; i < p->num_halo_files; i ++){
    rockstar[0][i] = load_rockstar_bin(p->halos_256[i]);
    rockstar[1][i] = load_rockstar_bin(p->halos_512[i]);
    rockstar[2][i] = load_rockstar_bin(p->halos_1024[i]);
    progress = simple_loading(progress, i, p->num_halo_files - 1);
  }
  done(_l_h_);

  // Generating triple cascade
  clock_t _g_t_c_ = start("Generating triple cascade of matching halos");
  vector **cascades = triplecascade(matches_512_256,
                                    matches_1024_512,
                                    rockstar[0][p->num_halo_files - 1]->header->num_halos,
                                    matches_256,
                                    matches_512,
                                    matches_1024,
                                    p->num_match_files);
  done(_g_t_c_);

  // Add modules here to study the evolution of the halo properties along the
  // history of the simulations
  clock_t _a_h_ = start("Generating accretion history");
  accretion_history(rockstar, cascades, p);
  done(_a_h_);

  // Cleaning up...
  clock_t _c_u_ = start("Cleaning Up...");

  dispose_triplecascade(&cascades);

  for(i = 0; i < p->num_halo_files; i++){
    dispose_halofinder(&rockstar[0][i]);
    dispose_halofinder(&rockstar[1][i]);
    dispose_halofinder(&rockstar[2][i]);
  }

  free(rockstar[0]);
  free(rockstar[1]);
  free(rockstar[2]);

  for(i = 0; i < p->num_match_files; i++){
    dispose_avltree(&matches_256[i]);
    dispose_avltree(&matches_512[i]);
    dispose_avltree(&matches_1024[i]);
  }

  free(matches_256);
  free(matches_512);
  free(matches_1024);

  dispose_avltree(&matches_512_256);
  dispose_avltree(&matches_1024_512);

  dispose_params(&p);
  done(_c_u_);

  return 0;
}
