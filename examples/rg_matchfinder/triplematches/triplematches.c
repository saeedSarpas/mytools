/*
 * examples/rg_mmatchfinder/triplematches/triplematches.c
 *
 * Finding matches between 3 different runs
 *
 * author: Saeed Sarpas
 */


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "./../triplecascade/triplecascade.h"
#include "./../../matchinghalos/load_mh.h"
#include "./../../../memory/allocate.h"
#include "./../../../configfile/mylibconfig.h"
#include "./../../../ll/ll.h"
#include "./../../../io/open_file.h"
#include "./../../../io/check_file.h"
#include "./../../../io/check_dir.h"
#include "./../../../io/check_dir.h"
#include "./../../../strings/concat/concat.h"
#include "./../../../time/mytime.h"
#include "./../../../avltree/avltree.h"
#include "./../../../avltree/avl_find.h"
#include "./../../../vector/vector.h"
#include "./../../../vector/vector_push.h"
#include "./../../../vector/vector_get.h"
#include "./../../../halofinder/halofinder.h"
#include "./../../../halofinder/rockstar/load_rockstar_bin.h"
#include "./../../../simulation/snapshot.h"
#include "./../../../simulation/gadget/load_gadget_snap.h"
#include "./../../../simulation/load_snap_from.h"
#include "./../../../simulation/save_snap_to.h"
#include "./../../../simulation/check_if_saved_exist.h"


#include "./triplematches.h"


int main(int argc, char *argv[])
{
  if(argc != 2){
    printf("[Error] You should specify the path to the configuration file.\n");
    printf("        ./triplematches /path/to/the/configuration/file\n");
    exit(EXIT_FAILURE);
  }

  triplematches_params *p = load_params(argv[1]);

  clock_t _l_m_h_ = start("Loading matching halo files");
  avltree *matches_256_512 = load_mh(p->matches_256_512);
  avltree *matches_512_1024 = load_mh(p->matches_512_1024);
  done(_l_m_h_);

  clock_t _l_h_ = start("Loading halo files");
  halofinder *halos_256 = load_rockstar_bin(p->halos_256);
  halofinder *halos_512 = load_rockstar_bin(p->halos_512);
  halofinder *halos_1024 = load_rockstar_bin(p->halos_1024);
  done(_l_h_);

  clock_t _l_i_c_ = start("Loading initial condition files\n");
  snapshot *ic_256, *ic_512, *ic_1024;
  ic_256 = safe_snapshot_load(p->ic_256);
  ic_512 = safe_snapshot_load(p->ic_512);
  ic_1024 = safe_snapshot_load(p->ic_1024);
  done(_l_i_c_);

  clock_t _l_s_ = start("Loading snapshots\n");
  snapshot *snap_256, *snap_512, *snap_1024;
  snap_256 = safe_snapshot_load(p->snap_256);
  snap_512 = safe_snapshot_load(p->snap_512);
  snap_1024 = safe_snapshot_load(p->snap_1024);
  done(_l_s_);

  // Finding matching halos
  clock_t _f_m_h_ = start("Finding matching halos");
  vector **cascades = triplecascade(matches_256_512, matches_512_1024,
      NULL, NULL, NULL, 0);
  done(_f_m_h_);

  // Saving output files
  clock_t _s_o_ = start("Saving output files");

  int pri_id, sec_id, tri_id, part_id;
  char fmt[] = "%17.15e\t%17.15e\t%17.15e\n";

  int i, j;
  for(i = 0; i < halos_256->header->num_halos; i++){
    if(cascades[i]->logLength != 3) continue;

    char fname[1024];
    pri_id = ((ll*)vector_get(cascades[i], 0))->head->ikey;
    sec_id = ((ll*)vector_get(cascades[i], 1))->head->ikey;
    tri_id = ((ll*)vector_get(cascades[i], 2))->head->ikey;

    sprintf(fname, "%s%d-%d-%d.dat", p->output_dir, pri_id, sec_id, tri_id);

    FILE *fp = open_file(fname, "w");

    fprintf(fp, "pri_halo_id: %d\n", pri_id);
    fprintf(fp, "pri_halo_mass: %15.13e\n", halos_256->halos[pri_id].m);
    fprintf(fp, "pri_halo_num_p: %d\n", (int)halos_256->halos[pri_id].num_p);

    fprintf(fp, "sec_halo_id: %d\n", sec_id);
    fprintf(fp, "sec_halo_mass: %15.13e\n", halos_512->halos[sec_id].m);
    fprintf(fp, "sec_halo_num_p: %d\n", (int)halos_512->halos[sec_id].num_p);

    fprintf(fp, "tri_halo_id: %d\n", tri_id);
    fprintf(fp, "tri_halo_mass: %15.13e\n", halos_1024->halos[tri_id].m);
    fprintf(fp, "tri_halo_num_p: %d\n", (int)halos_1024->halos[tri_id].num_p);

    fprintf(fp, "256_512_goodness: %f\n",
        ((match*)((avl_find(matches_256_512, &pri_id))->data))->goodness);
    fprintf(fp, "512_1024_goodness: %f\n",
        ((match*)((avl_find(matches_512_1024, &sec_id))->data))->goodness);

    // ic 256
    for(j = 0; j < halos_256->halos[pri_id].num_p; j++){
      part_id = halos_256->halos[pri_id].particle_ids[j];
      fprintf(fp, fmt, ic_256->particles[part_id].pos[0],
          ic_256->particles[part_id].pos[1],
          ic_256->particles[part_id].pos[2]);
    }

    // ic 512
    for(j = 0; j < halos_512->halos[sec_id].num_p; j++){
      part_id = halos_512->halos[sec_id].particle_ids[j];
      fprintf(fp, fmt, ic_512->particles[part_id].pos[0],
          ic_512->particles[part_id].pos[1],
          ic_512->particles[part_id].pos[2]);
    }

    // ic 1024
    for(j = 0; j < halos_1024->halos[tri_id].num_p; j++){
      part_id = halos_1024->halos[tri_id].particle_ids[j];
      fprintf(fp, fmt, ic_1024->particles[part_id].pos[0],
          ic_1024->particles[part_id].pos[1],
          ic_1024->particles[part_id].pos[2]);
    }

    // snap 256
    for(j = 0; j < halos_256->halos[pri_id].num_p; j++){
      part_id = halos_256->halos[pri_id].particle_ids[j];
      fprintf(fp, fmt, snap_256->particles[part_id].pos[0],
          snap_256->particles[part_id].pos[1],
          snap_256->particles[part_id].pos[2]);
    }

    // snap 512
    for(j = 0; j < halos_512->halos[sec_id].num_p; j++){
      part_id = halos_512->halos[sec_id].particle_ids[j];
      fprintf(fp, fmt, snap_512->particles[part_id].pos[0],
          snap_512->particles[part_id].pos[1],
          snap_512->particles[part_id].pos[2]);
    }

    // snap 1024
    for(j = 0; j < halos_1024->halos[tri_id].num_p; j++){
      part_id = halos_1024->halos[tri_id].particle_ids[j];
      fprintf(fp, fmt, snap_1024->particles[part_id].pos[0],
          snap_1024->particles[part_id].pos[1],
          snap_1024->particles[part_id].pos[2]);
    }

    fclose(fp);
  }
  done(_s_o_);

  // Cleaning up
  clock_t _c_u_ = start("Cleaning up...");

  ll *list = NULL;
  for(i = 0; i < halos_256->header->num_halos; i++){
    for(j = 0; j < cascades[i]->logLength; j++){
      list = vector_get(cascades[i], j);
      dispose_ll(&list);
    }
  }

  dispose_vector(&cascades[i]);

  free(cascades);

  dispose_avltree(&matches_256_512);
  dispose_avltree(&matches_512_1024);

  dispose_halofinder(&halos_256);
  dispose_halofinder(&halos_512);
  dispose_halofinder(&halos_1024);

  dispose_snapshot(&ic_256);
  dispose_snapshot(&ic_512);
  dispose_snapshot(&ic_1024);

  dispose_snapshot(&snap_256);
  dispose_snapshot(&snap_512);
  dispose_snapshot(&snap_1024);

  dispose_params(p);

  done(_c_u_);

  return 0;
}


/*
 * Loading parameters from config file
 */
static void check_file_harsh(char*, char*);
static triplematches_params* load_params(const char *path)
{
  triplematches_params *p = allocate(1, sizeof(*p));

  config_t *cfg = new_cfg();
  cfg_loadfile(cfg, path);

  const char *input, *output;

  config_setting_t *inputs = cfg_findsetting(cfg, "inputs");

  input = cfg_getstring(inputs, "matches_256_512");
  p->matches_256_512 = strdup(input);
  check_file_harsh(p->matches_256_512, "Wrong matches_256_512 in configs");

  input = cfg_getstring(inputs, "matches_512_1024");
  p->matches_512_1024 = strdup(input);
  check_file_harsh(p->matches_512_1024, "Wrong matches_512_1024 in configs");

  input = cfg_getstring(inputs, "halos_256");
  p->halos_256 = strdup(input);
  check_file_harsh(p->halos_256, "Wrong halos_256 in configs");

  input = cfg_getstring(inputs, "halos_512");
  p->halos_512 = strdup(input);
  check_file_harsh(p->halos_512, "Wrong halos_512 in configs");

  input = cfg_getstring(inputs, "halos_1024");
  p->halos_1024 = strdup(input);
  check_file_harsh(p->halos_1024, "Wrong halos_1024 in configs");

  input = cfg_getstring(inputs, "ic_256");
  p->ic_256 = strdup(input);
  check_file_harsh(p->ic_256, "Wrong ic_1024 in configs");

  input = cfg_getstring(inputs, "ic_512");
  p->ic_512 = strdup(input);
  check_file_harsh(p->ic_512, "Wrong ic_1024 in configs");

  input = cfg_getstring(inputs, "ic_1024");
  p->ic_1024 = strdup(input);
  check_file_harsh(p->ic_1024, "Wrong ic_1024 in configs");

  input = cfg_getstring(inputs, "snap_256");
  p->snap_256 = strdup(input);
  check_file_harsh(p->snap_256, "Wrong snap_256 in configs");

  input = cfg_getstring(inputs, "snap_512");
  p->snap_512 = strdup(input);
  check_file_harsh(p->snap_512, "Wrong snap_512 in configs");

  input = cfg_getstring(inputs, "snap_1024");
  p->snap_1024 = strdup(input);
  check_file_harsh(p->snap_1024, "Wrong snap_1024 in configs");

  config_setting_t *outputs = cfg_findsetting(cfg, "output");
  output = cfg_getstring(outputs, "dir");
  p->output_dir = strdup(output);

  if(check_dir(p->output_dir) != CD_DIR_EXIST){
    printf("[Error! Wrong (output) )dir in configs]\n");
    exit(EXIT_FAILURE);
  }

  return p;
}


/*
 * Disposing params structure
 */
static void dispose_params(triplematches_params *p)
{
  free(p->matches_256_512);
  free(p->matches_512_1024);
  free(p->halos_256);
  free(p->halos_512);
  free(p->halos_1024);
  free(p->ic_256);
  free(p->ic_512);
  free(p->ic_1024);
  free(p->snap_256);
  free(p->snap_512);
  free(p->snap_1024);
  free(p->output_dir);
}


static void check_file_harsh(char *path, char *error)
{
  if(check_file(path) != CF_FILE_EXIST){
    printf("[Error! %s]\n", error);
    exit(EXIT_FAILURE);
  }
}


static snapshot* safe_snapshot_load(char *path)
{
  if(check_if_saved_exist(path)){
    char msg[1024];
    sprintf(msg, "\t[Saved version exist for %s]", path);
    clock_t _s_e_ = start(msg);
    snapshot *snap = load_snap_from(saved_snap_path(path));
    done(_s_e_);
    return snap;
  }
  else{
    char msg[1024];
    sprintf(msg, "\t[Loading %s]", path);
    clock_t _s_e_ = start(msg);
    snapshot *snap = load_gadget_snap(path);
    save_snap_to(saved_snap_path(path), snap);
    done(_s_e_);
    return snap;
  }
}
