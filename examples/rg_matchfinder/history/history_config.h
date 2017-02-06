#ifndef HISTORY_CONFIG_H
#define HISTORY_CONFIG_H


typedef struct _mh_history_params
{
  char *matches_512_256, *matches_1024_512;
  char **halos_256, **halos_512, **halos_1024;
  char **matches_256, **matches_512, **matches_1024;
  int num_match_files, num_halo_files;
  char *ascii_output;
} mh_history_params;


mh_history_params* load_params(const char*);
void dispose_params(mh_history_params**);


#endif /* HISTORY_CONFIG_H */
