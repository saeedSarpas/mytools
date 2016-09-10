#ifndef SORT_ROCKSTAR_HALOS_H
#define SORT_ROCKSTAR_HALOS_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./rockstar_data_type.h"


void sort_rockstar_halos(halo*, int64_t,
                         int (*compare_func)(const void*, const void*));
int compare_mass(const void*, const void*);
int compare_id(const void*, const void*);


#endif /* SORT_ROCKSTAR_HALOS_H */
