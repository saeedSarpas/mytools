#ifndef FITYAX_H
#define FITYAX_H


#include "./../../vector/vector.h"

typedef struct _fitresultyax {
  double slope;
} fitresultyax;


fitresultyax* fityax(vector*, vector*);


#endif /* FITYAX_H */
