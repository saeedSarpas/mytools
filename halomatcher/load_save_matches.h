#ifndef LOAD_SAVE_MATCHES_H
#define LOAD_SAVE_MATCHES_H


#include "./../vector/vector.h"
#include "./../halofinder/halofinder.h"


typedef struct _matchesfileheader
{
  int nprihalos;
  int nsechalos;
} matchesfileheader;


void load_matches(char*, vector**, vector**);
void write_matches(char*, vector**, vector**, int, int);


#endif /* LOAD_SAVE_MATCHES_H */
