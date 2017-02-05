#ifndef FITYEAX_H
#define FITYEAX_H


#include "./../../vector/vector.h"

typedef struct _fitresultyeax {
  float slope;
} fitresultyeax;


fitresultyeax* fityeax(vector*, vector*);


#endif /* FITYEAX_H */
