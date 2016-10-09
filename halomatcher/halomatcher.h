#ifndef HALOMATCHER_H
#define HALOMATCHER_H


#include "./matchinghalo.h"
#include "./../halofinder/halofinder.h"


typedef struct _halomatcher_params
{
  float massoffset;
  float mingoodness;
} halomatcher_params;


matchinghalo* halomatcher(halofinder*, halofinder*, halomatcher_params);


#endif /* HALOMATCHER_H */
