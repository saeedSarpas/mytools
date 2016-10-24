#ifndef SINGLEHALO_MATCHER_H
#define SINGLEHALO_MATCHER_H


#include "./matchinghalo.h"
#include "./../halofinder/halofinder.h"
#include "./../vector/vector.h"


typedef struct _halomatcher_params
{
  float massOffset;
  float maxDisplacement;
  int loadMatches;
  char *loadMatchesPath;
  int saveMatches;
  char *saveMatchesPath;
} halomatcher_params;


typedef struct _match
{
  int matchid;
  float goodness;
} match;


vector* singlehalo_matcher(halo*, halofinder*, halomatcher_params);


#endif /* SINGLEHALO_MATCHER_H */
