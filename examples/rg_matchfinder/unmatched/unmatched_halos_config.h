#ifndef UNMATCHED_HALOS_CONFIG_H
#define UNMATCHED_HALOS_CONFIG_H


typedef struct _unmatched_halos_params
{
  char* matched_catalog;
  char* halos256, *halos512, *halos1024;
  char* snap256, *snap512, *snap1024;
  int window_len;
  char* output_dir;
} unmatched_halos_params;


unmatched_halos_params* load_unmatched_halos_params(const char*);


#endif /* UNMATCHED_HALOS_CONFIG_H */
