/*
 * halomatcher/matchinghalo.c
 * test_file: halomatcher/matchinghalo_test.c
 *
 * creating and destroying generic matchinghalo structure
 *
 * param: len the length of the array
 *
 * return: a matchinghalo structure of array
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include "matchinghalo.h"
#include "./../memory/allocate.h"


/*
 * creating a new matchinghalo structure
 *
 * param: len the length of the arrays inside matchinghalo structure
 *
 * return: allocated matchinghalo structure
 */
matchinghalo* new_matchinghalo(int len)
{
  matchinghalo *mh = allocate(1, sizeof(matchinghalo));
  mh->len = len;
  mh->matchingids = allocate(len, sizeof(int));
  mh->goodnesses = allocate(len, sizeof(float));

  int i;
  for(i = 0; i < mh->len; i++){
    mh->matchingids[i] = MATCHINGHALONOTSET;
    mh->goodnesses[i] = 0.0;
  }

  return mh;
}


/*
 * disposing a given array of matchinghalo structures
 */
void dispose_matchinghalo(matchinghalo *mh)
{
  free(mh->matchingids);
  mh->matchingids = NULL;

  free(mh->goodnesses);
  mh->goodnesses = NULL;

  free(mh);
  mh = NULL;
}
