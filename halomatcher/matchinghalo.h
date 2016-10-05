#ifndef MATCHINGHALO_H
#define MATCHINGHALO_H


#define MATCHINGHALONOTSET -1


typedef struct _matchinghalo {
  int len;
  int *matchingids;
  float *goodnesses;
  void (*dispose)(struct _matchinghalo*);
} matchinghalo;


matchinghalo* new_matchinghalo(int);


#endif /* MATCHINGHALO_H */
