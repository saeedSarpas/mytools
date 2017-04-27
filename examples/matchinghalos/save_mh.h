#ifndef SAVE_MH_H
#define SAVE_MH_H


#include "./../../halofinder/halofinder.h"
#include "./../../halomatcher/matchinghalo.h"
#include "./../rg_matchfinder/matchfinder.h"


#define HEADER_LENGTH 9


void save_mh(halofinder*, halofinder*, matchfinder_params*, matchinghalo*);


#endif /* SAVE_MH_H */
