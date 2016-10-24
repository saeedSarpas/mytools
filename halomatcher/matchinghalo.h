#ifndef MATCHINGHALO_H
#define MATCHINGHALO_H


#define MATCHINGHALONOTSET -1


typedef struct _matchinghalo {
  int len;
  int *matchingids;
  float *goodnesses;
} matchinghalo;


matchinghalo* new_matchinghalo(int);
void dispose_matchinghalo(matchinghalo*);


#endif /* MATCHINGHALO_H */
