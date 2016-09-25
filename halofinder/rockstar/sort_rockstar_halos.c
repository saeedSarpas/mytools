/*
 * halofinder/sort_rockstar_halos.c
 * test_file: halofinder/sort_rockstar_halos_test.c
 *
 * Sorting rockstar halos based on their different attributes
 *
 * param: halos pointer to rockstar halos
 * param: n total number of elements
 * param: ref refrence attribute for sorting
 *
 * TODO: using enum instead of specifying the field for sorting (e.g. id, m)
 *
 * author: Saeed Sarpas
 */


#include "sort_rockstar_halos.h"


void sort_rockstar_halos(struct halo *halos, int64_t n,
                         int (*compare_func)(const void*, const void*)){
  qsort((void*) halos, n, sizeof(struct halo), compare_func);
}


int compare_mass(const void *h1, const void *h2){
  const struct halo *elem1 = h1;
  const struct halo *elem2 = h2;

  if (elem1->m < elem2->m)
    return -1;
  else if (elem1->m > elem2->m)
    return 1;
  else
    return 0;
}
