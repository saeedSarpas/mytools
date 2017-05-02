#ifndef UNMATCHED_HALOS_CONFIG_H
#define UNMATCHED_HALOS_CONFIG_H


typedef struct _unmatched_halos_params
{
  char* matched_catalog;
  char* pri_halos;
  char* pri_snap, *sec_snap;
  int box_size;
  char* output_dir;
} unmatched_halos_params;


unmatched_halos_params* load_unmatched_halos_params(const char*);


#endif /* UNMATCHED_HALOS_CONFIG_H */
