/**
 * examples/rg_matchfinder/unmatched/unmatched_halos.c
 *
 * Generating data necessary for visualizing unmatched halos
 *
 * author: Saeed Sarpas
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* #include <omp.h> */
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
#include "./../../../memory/copy.h"
#include "./../../../avltree/avl_inorder_traversal.h"
#include "./../../../io/open_file.h"
#include "./../../../search/half_interval/find_ge.h"
#include "./../../../vector/vector.h"
#include "./../../../vector/vector_push.h"


#define ADDRLEN 1024
#define MSGLEN 1024


struct address_t{
  char header[ADDRLEN];
  char halo[ADDRLEN];
  char particles[ADDRLEN][3];
  char halos[ADDRLEN][3];
} address;


static snapshot* safe_snapshot_load(char*);
static int sort_particles_based_on_x (const void*, const void*);
static int sort_halos_based_on_x (const void*, const void*);
static int compare_particles_based_on_x (const void*, int, const void*);
static int compare_halos_based_on_x (const void*, int, const void*);
static void set_matched_list(avlnode*, void*);
static void calculate_window(float[3][2], float[3][2], halo*,
                             float, unmatched_halos_params*);
static void find_particles_in_window(vector**, snapshot**, float[3][2]);
static void find_halos_in_window(vector**, halofinder**, float[3][2]);
static void fill_address(struct address_t*, unmatched_halos_params*, int);
static void print_header(FILE*, unmatched_halos_params*, struct address_t*,
                         halo*, float[3][2], vector**, vector**);
static void print_halo(FILE*, halo*, snapshotparticle*);
static void print_particles(FILE*, vector*, snapshot*);
static void print_halos(FILE*, vector*, halofinder*);


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


  clock_t _l_h_ = start("Loading halos files");
  halofinder* rockstar_id[3], *rockstar_x[3];
  rockstar_id[0] = load_rockstar_bin(p->halos256);
  rockstar_id[1] = load_rockstar_bin(p->halos512);
  rockstar_id[2] = load_rockstar_bin(p->halos1024);

  for (int i = 0; i < 3; i++)
    {
      rockstar_x[i] = allocate(1, sizeof(halofinder));

      rockstar_x[i]->header = allocate(1, sizeof(haloheader));
      copy(rockstar_id[i]->header, rockstar_x[i]->header, sizeof(haloheader));

      int num_halos = (int)rockstar_id[i]->header->num_halos;
      rockstar_x[i]->halos = allocate(num_halos, sizeof(halo));
      copy(rockstar_id[i]->halos, rockstar_x[i]->halos, num_halos * sizeof(halo));
    }
  done(_l_h_);


  clock_t _s_h_ = start("Sorting halos based on x axis");
  for (int i = 0; i < 3; i++)
    {
      qsort(
        rockstar_x[i]->halos,
        rockstar_x[i]->header->num_halos,
        sizeof(halo),
        sort_halos_based_on_x);
    }
  done(_s_h_);


  clock_t _l_s_ = start("loading snapshots\n");
  snapshot* snaps_x[3], *snaps_id[3];
  snaps_id[0] = safe_snapshot_load(p->snap256);
  snaps_id[1] = safe_snapshot_load(p->snap512);
  snaps_id[2] = safe_snapshot_load(p->snap1024);

  for (int i = 0; i < 3; i++)
    {
      snaps_x[i] = allocate(1, sizeof(snapshot));

      snaps_x[i]->header = allocate(1, sizeof(snapshotheader));
      copy(snaps_id[i]->header, snaps_x[i]->header, sizeof(snapshotheader));

      int num_particles = snaps_id[i]->header->tot_nparticles;
      snaps_x[i]->particles = allocate(num_particles, sizeof(snapshotparticle));
      copy(snaps_id[i]->particles, snaps_x[i]->particles, num_particles * sizeof(snapshotparticle));
      copy(snaps_id[i], snaps_x[i], sizeof(*snaps_id[i]));
    }
  done(_l_s_);


  clock_t _s_s_ = start("Sorting snapshots based on x axis");
  for (int i = 0; i < 3; i++)
    {
      qsort(
        snaps_x[i]->particles,
        snaps_x[i]->header->tot_nparticles,
        sizeof(snapshotparticle),
        sort_particles_based_on_x);
    }
  done(_s_s_);


  clock_t _f_u_h_ = start("Finding unmatched halos");
  int* matched_list = allocate(rockstar_id[0]->header->num_halos, sizeof(int));
  for (int i = 0; i < rockstar_id[0]->header->num_halos; i++)
    matched_list[i] = 0;

  avl_inorder_traversal(matched_catalog, set_matched_list, matched_list);
  done(_f_u_h_);


  clock_t _s_c_p_ = start("Start calculating prerequisites values");
  float largest_rvir = 0;
  for (int i = 0; i < rockstar_id[2]->header->num_halos; i++)
    if (rockstar_id[2]->halos[i].r > largest_rvir)
      largest_rvir = rockstar_id[2]->halos[i].r;
  done(_s_c_p_);


  clock_t _s_p_ = start("Saving particles\n");
#pragma omp parallel
  for (int h_id = 0; h_id < rockstar_id[0]->header->num_halos; h_id++)
    {
      if (matched_list[h_id] == 1) // There has been a matched halos for this one
        continue;

      float window[3][2], halos_window[3][2];
      calculate_window(
        window,
        halos_window,
        &rockstar_id[0]->halos[h_id],
        largest_rvir,
        p);


      clock_t _f_p_h_ = start("\tFinding particles and halos inside the window");
      vector* particles_in_window[3], *halos_in_window[3];
      find_particles_in_window(particles_in_window, snaps_x, window);
      find_halos_in_window(halos_in_window, rockstar_x, halos_window);
      done(_f_p_h_);


      fill_address(&address, p, h_id);


      clock_t _w_o_ = start("\tWriting output files");
      FILE* header_file = open_file(address.header, "w");
      print_header(header_file, p, &address, &rockstar_id[0]->halos[h_id],
                   window, particles_in_window, halos_in_window);
      fclose(header_file);

      FILE* halo_file = open_file(address.halo, "w");
      print_halo(halo_file, &rockstar_id[0]->halos[h_id], snaps_id[0]->particles);
      fclose(halo_file);

      FILE* particles_in_window_file[3], *halos_in_window_file[3];
      for (int i = 0; i < 3; i++)
        {
          particles_in_window_file[i] = open_file(address.particles[i], "w");
          print_particles(
            particles_in_window_file[i],
            particles_in_window[i],
            snaps_id[i]);
          fclose(particles_in_window_file[i]);

          halos_in_window_file[i] = open_file(address.halos[i], "w");
          print_halos(
            halos_in_window_file[i],
            halos_in_window[i],
            rockstar_id[i]);
          fclose(halos_in_window_file[i]);
        }

      done(_w_o_);

      for (int i = 0; i < 3; i++)
        {
          dispose_vector(&particles_in_window[i]);
          dispose_vector(&halos_in_window[i]);
        }
    }

  done(_s_p_);

  // OS will free all allocated memory for us :D

  return 0;
}


static snapshot* safe_snapshot_load(char* path)
{
  if(check_if_saved_exist(path)){
    char msg[MSGLEN];
    sprintf(msg, "\t[Saved version exist for %s]", path);
    clock_t _s_e_ = start(msg);
    snapshot *snap = load_snap_from(saved_snap_path(path));
    done(_s_e_);
    return snap;
  }
  else{
    char msg[MSGLEN];
    sprintf(msg, "\t[Loading %s]", path);
    clock_t _s_e_ = start(msg);
    snapshot *snap = load_gadget_snap(path);
    save_snap_to(saved_snap_path(path), snap);
    done(_s_e_);
    return snap;
  }
}


static int sort_particles_based_on_x (const void* particle1,
                                      const void* particle2)
{
  const snapshotparticle* particle1_c = particle1;
  const snapshotparticle* particle2_c = particle2;

  if (particle1_c->pos[0] < particle2_c->pos[0])
    return -1;
  else if (particle1_c->pos[0] > particle2_c->pos[0])
    return 1;
  else
    return 0;
}


static int sort_halos_based_on_x (const void* halo1, const void* halo2)
{
  const halo* halo1_c = halo1;
  const halo* halo2_c = halo2;

  if (halo1_c->pos[0] < halo2_c->pos[0])
    return -1;
  else if (halo1_c->pos[0] > halo2_c->pos[0])
    return 1;
  else
    return 0;
}


static int compare_particles_based_on_x (
  const void* particles,
  int index,
  const void* target_x)
{
  const float x1 = ((snapshotparticle*)particles)[index].pos[0];
  const float x2 = *(float*)target_x;

  if(x1 < x2)
    return LESS_THAN_T_VALUE;
  else if(x1 > x2)
    return GREATER_THAN_T_VALUE;
  else
    return EQUAL_TO_T_VALUE;
}


static int compare_halos_based_on_x (
  const void* halos,
  int index,
  const void* target_x)
{
  const float x1 = ((halo*)halos)[index].pos[0];
  const float x2 = *(float*)target_x;

  if (x1 < x2)
    return LESS_THAN_T_VALUE;
  else if (x1 > x2)
    return GREATER_THAN_T_VALUE;
  else
    return EQUAL_TO_T_VALUE;
}


static void set_matched_list(avlnode* node, void* matchedlist)
{
  ((int*)matchedlist)[*(int*)node->key] = 1;
}


static void print_header(
  FILE* file,
  unmatched_halos_params* p,
  struct address_t* address,
  halo* halo,
  float window[3][2],
  vector** particles,
  vector** halos)
{
  /* Printing the header of the output file */
  fprintf(file, "header_output_addr: %s\n", address->header);
  fprintf(file, "halo_output_addr: %s\n", address->halo);
  fprintf(file, "particles256_output_addr: %s\n", address->particles[0]);
  fprintf(file, "particles512_output_addr: %s\n", address->particles[1]);
  fprintf(file, "particles1024_output_addr: %s\n", address->particles[2]);
  fprintf(file, "halos256_output_addr: %s\n", address->halos[0]);
  fprintf(file, "halos512_output_addr: %s\n", address->halos[1]);
  fprintf(file, "halos1024_output_addr: %s\n", address->halos[2]);
  fprintf(file, "matched_catalog: %s\n", p->matched_catalog);
  fprintf(file, "halos256_input_addr: %s\n", p->halos256);
  fprintf(file, "halos512_input_addr: %s\n", p->halos512);
  fprintf(file, "halos1024_input_addr: %s\n", p->halos1024);
  fprintf(file, "snap256_input_addr: %s\n", p->snap256);
  fprintf(file, "snap512_input_addr: %s\n", p->snap512);
  fprintf(file, "snap1024_input_addr: %s\n", p->snap1024);
  fprintf(file, "window_len: %d\n", p->window_len);
  fprintf(file, "window_len_unit: halo_r\n");
  fprintf(file, "output_dir: %s\n", p->output_dir);
  fprintf(file, "halo_id: %d\n", halo->id);
  fprintf(file, "halo_x: %8.6f\n", halo->pos[0]);
  fprintf(file, "halo_y: %8.6f\n", halo->pos[1]);
  fprintf(file, "halo_z: %8.6f\n", halo->pos[2]);
  fprintf(file, "halo_vx: %8.6f\n", halo->vel[0]);
  fprintf(file, "halo_vy: %8.6f\n", halo->vel[1]);
  fprintf(file, "halo_vz: %8.6f\n", halo->vel[2]);
  fprintf(file, "halo_num_p: %d\n", (int)halo->num_p);
  fprintf(file, "halo_r: %8.6f\n", halo->r);
  fprintf(file, "xmin: %8.6f\n", window[0][0]);
  fprintf(file, "xmax: %8.6f\n", window[0][1]);
  fprintf(file, "ymin: %8.6f\n", window[1][0]);
  fprintf(file, "ymax: %8.6f\n", window[1][1]);
  fprintf(file, "zmin: %8.6f\n", window[2][0]);
  fprintf(file, "zmax: %8.6f\n", window[2][1]);
  fprintf(file, "particles256_found: %d\n", particles[0]->logLength);
  fprintf(file, "particles512_found: %d\n", particles[1]->logLength);
  fprintf(file, "particles1024_found: %d\n", particles[2]->logLength);
  fprintf(file, "halos256_found: %d\n", halos[0]->logLength);
  fprintf(file, "halos512_found: %d\n", halos[1]->logLength);
  fprintf(file, "halos1024_found: %d\n", halos[2]->logLength);
}


static void print_halo(FILE* file, halo* halo, snapshotparticle* particles)
{
  fprintf(file, "%8s\t%8s\t%8s\t%8s\n", "id", "x", "y", "z");

  int p_id;
  snapshotparticle* p;
  for (int i = 0; i < halo->num_p; i++)
    {
      p_id = halo->particle_ids[i];
      p = &particles[p_id];

      fprintf(file, "%08ld\t%08.6e\t%08.6e\t%08.6e\n",
              p->id, p->pos[0], p->pos[1], p->pos[2]);
    }
}


static void print_particles(FILE* file, vector* particles, snapshot* snaps_id)
{
  int* p_ids = particles->elems;
  snapshotparticle* ps = snaps_id->particles;

  fprintf(file, "%8s\t%8s\t%8s\t%8s\n", "id", "x", "y", "z");

  for (unsigned int j = 0; j < particles->logLength; j++)
    fprintf(
      file,
      "%08ld\t%08.6e\t%08.6e\t%08.6e\n",
      ps[p_ids[j]].id,
      ps[p_ids[j]].pos[0],
      ps[p_ids[j]].pos[1],
      ps[p_ids[j]].pos[2]);
}


static void print_halos(FILE* file, vector* halos, halofinder* rockstar_id)
{
  int* h_ids = halos->elems;
  halo* hs = rockstar_id->halos;

  fprintf(file, "%8s\t%8s\t%8s\t%8s\t%8s\n", "id", "x", "y", "z", "r");

  for (unsigned int j = 0; j < halos->logLength; j++)
    fprintf(
      file,
      "%08d\t%08.6e\t%08.6e\t%08.6e\t%08.6e\n",
      hs[h_ids[j]].id,
      hs[h_ids[j]].pos[0],
      hs[h_ids[j]].pos[1],
      hs[h_ids[j]].pos[2],
      hs[h_ids[j]].r);
}


static void calculate_window(
  float window[3][2],
  float halos_window[3][2],
  halo* halo,
  float largest_rvir,
  unmatched_halos_params* p)
{
  // halo positions are in the unit of h^-1 Mpc
  // halo Virial radii are in the unit of h^-1 kpc
  for (int i = 0; i < 3; i++)
    {
      window[i][0] = halo->pos[i] * 1000 - p->window_len * halo->r;
      window[i][1] = halo->pos[i] * 1000 + p->window_len * halo->r;

      halos_window[i][0] = window[i][0] - largest_rvir;
      halos_window[i][1] = window[i][1] + largest_rvir;
    }
}


static void find_particles_in_window(
  vector* particles[3],
  snapshot* snaps_x[3],
  float window[3][2])
{
  for (int i = 0; i < 3; i++)
    {
      particles[i] = new_vector(128 * pow(2, 3*i), sizeof(int));

      int index = find_ge(
        &window[0][0],
        snaps_x[i]->particles,
        snaps_x[i]->header->tot_nparticles,
        compare_particles_based_on_x);

      if (index == NOT_FOUND)
        continue;

      while (snaps_x[i]->particles[index].pos[0] < window[0][1] &&
             index < snaps_x[i]->header->tot_nparticles)
        {
          if (snaps_x[i]->particles[index].pos[1] >= window[1][0] &&
              snaps_x[i]->particles[index].pos[1] < window[1][1] &&
              snaps_x[i]->particles[index].pos[2] >= window[2][0] &&
              snaps_x[i]->particles[index].pos[2] < window[2][1])
            {
              vector_push(particles[i], &index);
            }

          index++;
        }
    }
}

static void find_halos_in_window(
  vector* halos[3],
  halofinder* rockstar_x[3],
  float window[3][2])
{
  for (int i = 0; i < 3; i++)
    {
      halos[i] = new_vector(16 * pow(2, 3*i), sizeof(int));

      int index = find_ge(
        &window[0][0],
        rockstar_x[i]->halos,
        rockstar_x[i]->header->num_halos,
        compare_halos_based_on_x);

      if (index == NOT_FOUND)
        continue;

      while (rockstar_x[i]->halos[index].pos[0] < window[0][1] &&
             index < rockstar_x[i]->header->num_halos)
        {
          if (rockstar_x[i]->halos[index].pos[1] >= window[1][0] &&
              rockstar_x[i]->halos[index].pos[1] < window[1][1] &&
              rockstar_x[i]->halos[index].pos[2] >= window[2][0] &&
              rockstar_x[i]->halos[index].pos[2] < window[2][1])
            {
              vector_push(halos[i], &index);
            }

          index++;
        }
    }
}


static void fill_address(
  struct address_t* address,
  unmatched_halos_params* p,
  int h_id)
{
  sprintf(address->header, "%s%07d_header.dat", p->output_dir, h_id);
  sprintf(address->halo, "%s%07d_halo.dat", p->output_dir, h_id);

  char* postfix[3] = {"256", "512", "1024"};
  for (int i = 0; i < 3; i++)
    {
      sprintf(address->particles[i], "%s%07d_particles_%s.dat",
              p->output_dir, h_id, postfix[i]);

      sprintf(address->halos[i], "%s%07d_halos_%s.dat",
              p->output_dir, h_id, postfix[i]);
    }
}
