/**
 * examples/rg_matchfinder/unmatched/unmatched_halos.c
 *
 * Generating data necessary for visualizing unmatched halos
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "unmatched_halos_config.h"
#include "./../../../time/mytime.h"
#include "./../../matchinghalos/load_mh.h"
#include "./../../../avltree/avltree.h"
#include "./../../../halofinder/halofinder.h"
#include "./../../../halofinder/rockstar/load_rockstar_bin.h"
#include "./../../../simulation/snapshot.h"
#include "./../../../simulation/load_snap_from.h"
#include "./../../../simulation/save_snap_to.h"
#include "./../../../simulation/check_if_saved_exist.h"
#include "./../../../simulation/gadget/load_gadget_snap.h"
#include "./../../../memory/allocate.h"
#include "./../../../avltree/avl_inorder_traversal.h"
#include "./../../../io/open_file.h"
#include "./../../../search/half_interval/find_ge.h"
#include "./../../../ll/ll.h"
#include "./../../../ll/ll_add.h"
#include "./../../../loading/simple_loading.h"


static snapshot* safe_snapshot_load(char*);
static int sort_based_on_x (const void*, const void*);
static void set_matched_list(avlnode*, void*);
static int compare_based_on_x (const void*, int, const void*);


int main (int argc, char *argv[])
{
  if (argc != 2)
    {
      printf("[Error] You should specify a path to a configuration file.\n");
      printf("        ./unmatched_halos /path/to/a/configuration/file\n");
      exit(EXIT_FAILURE);
    }

  unmatched_halos_params *p = load_unmatched_halos_params(argv[1]);

  clock_t _l_m_h_ = start("Loading matched halos");
  avltree* matched_catalog = load_mh(p->matched_catalog);
  done(_l_m_h_);

  clock_t _l_h_ = start("Loading primary and secondary halos files");
  halofinder* pri_halos = load_rockstar_bin(p->pri_halos);
  done(_l_h_);

  clock_t _l_s_ = start("loading primary and secondary snapshots\n");
  snapshot* pri_snap_sortedid = safe_snapshot_load(p->pri_snap);
  snapshot* pri_snap_sortedx = safe_snapshot_load(p->pri_snap);
  snapshot* sec_snap_sortedx = safe_snapshot_load(p->sec_snap);
  done(_l_s_);

  clock_t _s_s_ = start("Sorting primary and secondary snapshots based on x");
  qsort(pri_snap_sortedx->particles,
        pri_snap_sortedx->header->tot_nparticles,
        sizeof(snapshotparticle),
        sort_based_on_x);
  qsort(sec_snap_sortedx->particles,
        sec_snap_sortedx->header->tot_nparticles,
        sizeof(snapshotparticle),
        sort_based_on_x);
  done(_s_s_);

  clock_t _f_u_h_ = start("Finding unmatched halos");
  int* matched_list = allocate(pri_halos->header->num_halos, sizeof(int));
  for (int i = 0; i < pri_halos->header->num_halos; i++)
    matched_list[i] = 0;

  avl_inorder_traversal(matched_catalog, set_matched_list, matched_list);
  done(_f_u_h_);


  clock_t _s_p_ = start("Saving particles\n");
  int loading = -1;
  char address[1024];
#pragma omp parallel for private(loading)
  for (int i = 0; i < pri_halos->header->num_halos; i++)
    {
      loading = simple_loading(loading, i, pri_halos->header->num_halos - 1);

      if (matched_list[i] == 1)
        continue;

      /**
       * Halos positions are in the unit of Mpc/h but their radius is in Kpc/h
       * Snapshot particle positions are in the unit of Kpc/h
       */
      float box_x[2] = {
        pri_halos->halos[i].pos[0] * 1000 - p->box_size * pri_halos->halos[i].r,
        pri_halos->halos[i].pos[0] * 1000 + p->box_size * pri_halos->halos[i].r
      };

      float box_y[2] = {
        pri_halos->halos[i].pos[1] * 1000 - p->box_size * pri_halos->halos[i].r,
        pri_halos->halos[i].pos[1] * 1000 + p->box_size * pri_halos->halos[i].r
      };

      float box_z[2] = {
        pri_halos->halos[i].pos[2] * 1000 - p->box_size * pri_halos->halos[i].r,
        pri_halos->halos[i].pos[2] * 1000 + p->box_size * pri_halos->halos[i].r
      };

      int index = find_ge((void*)&box_x[0],
                          pri_snap_sortedx->particles,
                          pri_snap_sortedx->header->tot_nparticles,
                          compare_based_on_x);

      if (index == NOT_FOUND)
          continue;

      ll* pri_list_of_points = new_ll(LL_INT);

      while (pri_snap_sortedx->particles[index].pos[0] < box_x[1] &&
             index < pri_snap_sortedx->header->tot_nparticles)
        {
          if (pri_snap_sortedx->particles[index].pos[1] >= box_y[0] &&
              pri_snap_sortedx->particles[index].pos[1] < box_y[1] &&
              pri_snap_sortedx->particles[index].pos[2] >= box_z[0] &&
              pri_snap_sortedx->particles[index].pos[2] < box_z[1])
            {
              ll_add(pri_list_of_points, &index, LL_BACK, NULL);
            }

          index++;
        }

      index = find_ge((void*)&box_x[0],
                      sec_snap_sortedx->particles,
                      sec_snap_sortedx->header->tot_nparticles,
                      compare_based_on_x);

      if (index == NOT_FOUND)
        continue;

      ll* sec_list_of_points = new_ll(LL_INT);

      while (sec_snap_sortedx->particles[index].pos[0] < box_x[1] &&
             index < sec_snap_sortedx->header->tot_nparticles)
        {
          if (sec_snap_sortedx->particles[index].pos[1] >= box_y[0] &&
              sec_snap_sortedx->particles[index].pos[1] < box_y[1] &&
              sec_snap_sortedx->particles[index].pos[2] >= box_z[0] &&
              sec_snap_sortedx->particles[index].pos[2] < box_z[1])
            {
              ll_add(sec_list_of_points, &index, LL_BACK, NULL);
            }

          index++;
        }

      sprintf(address, "%s%07d.dat", p->output_dir, (int)pri_halos->halos[i].id);
      FILE* file = open_file(address, "w");

      /* Printing the header of the output file */
      fprintf(file, "matched_catalog: %s\n", p->matched_catalog);
      fprintf(file, "pri_halos: %s\n", p->pri_halos);
      fprintf(file, "pri_snap: %s\n", p->pri_snap);
      fprintf(file, "sec_snap: %s\n", p->sec_snap);
      fprintf(file, "output_dir: %s\n", p->output_dir);
      fprintf(file, "Primary halo id: %d\n", i);
      fprintf(file, "Primary halo x: %8.6f\n", pri_halos->halos[i].pos[0]);
      fprintf(file, "Primary halo y: %8.6f\n", pri_halos->halos[i].pos[1]);
      fprintf(file, "Primary halo z: %8.6f\n", pri_halos->halos[i].pos[2]);
      fprintf(file, "Primary halo r: %8.6f\n", pri_halos->halos[i].r);
      fprintf(file, "Box min x: %8.6f\n", box_x[0]);
      fprintf(file, "Box max x: %8.6f\n", box_x[1]);
      fprintf(file, "Box min y: %8.6f\n", box_y[0]);
      fprintf(file, "Box max y: %8.6f\n", box_y[1]);
      fprintf(file, "Box min z: %8.6f\n", box_z[0]);
      fprintf(file, "Box max z: %8.6f\n", box_z[1]);
      fprintf(file, "N_p primary halo: %d\n", (int)pri_halos->halos[i].num_p);
      fprintf(file, "N_p primary box: %d\n", pri_list_of_points->len);
      fprintf(file, "N_p secondary box: %d\n", sec_list_of_points->len);

      for (int j = 0; j < pri_halos->halos[i].num_p; j++)
        fprintf(file, "%8.6e\t%8.6e\t%8.6e\n",
                pri_snap_sortedid->particles[pri_halos->halos[i].particle_ids[j]].pos[0],
                pri_snap_sortedid->particles[pri_halos->halos[i].particle_ids[j]].pos[1],
                pri_snap_sortedid->particles[pri_halos->halos[i].particle_ids[j]].pos[2]);

      llnode* node = pri_list_of_points->head;
      while(node)
        {
          fprintf(file, "%8.6e\t%8.6e\t%8.6e\n",
                  pri_snap_sortedx->particles[node->ikey].pos[0],
                  pri_snap_sortedx->particles[node->ikey].pos[1],
                  pri_snap_sortedx->particles[node->ikey].pos[2]);

          node = node->next;
        }

      node = sec_list_of_points->head;
      while(node)
        {
          fprintf(file, "%8.6e\t%8.6e\t%8.6e\n",
                  sec_snap_sortedx->particles[node->ikey].pos[0],
                  sec_snap_sortedx->particles[node->ikey].pos[1],
                  sec_snap_sortedx->particles[node->ikey].pos[2]);

          node = node->next;
        }

      dispose_ll(&pri_list_of_points);
      dispose_ll(&sec_list_of_points);
      fclose(file);
    }
  done(_s_p_);

  return 0;
}


static snapshot* safe_snapshot_load(char* path)
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


static int sort_based_on_x (const void* pri_particle, const void* sec_particle)
{
  const snapshotparticle* pri_particle_c = pri_particle;
  const snapshotparticle* sec_particle_c = sec_particle;

  if (pri_particle_c->pos[0] < sec_particle_c->pos[0])
    return -1;
  else if (pri_particle_c->pos[0] > sec_particle_c->pos[0])
    return 1;
  else
    return 0;
}


static int compare_based_on_x (const void* particles,
                               int index, const void* target_value)
{
  const float value = ((snapshotparticle*)particles)[index].pos[0];
  const float t_value = *(float*)target_value;

  if(value < t_value)
    return LESS_THAN_T_VALUE;
  else if(value > t_value)
    return GREATER_THAN_T_VALUE;
  else
    return EQUAL_TO_T_VALUE;
}


static void set_matched_list(avlnode* node, void* matchedlist)
{
  ((int*)matchedlist)[*(int*)node->key] = 1;
}
