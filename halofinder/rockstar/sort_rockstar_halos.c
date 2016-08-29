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


int compare_mass(const void*, const void*);
int compare_id(const void*, const void*);


void sort_rockstar_halos(struct rockstar_halo *halos,
                         int64_t n, const char* ref){
  if (strcmp(ref, "m") == 0)
    qsort((void*) halos, n, sizeof(struct rockstar_halo), compare_mass);
  else if (strcmp(ref, "id") == 0)
    qsort((void*) halos, n, sizeof(struct rockstar_halo), compare_id);
  else
    printf("[sort_rockstar_halo error: Unknown refrence element");
}


int compare_mass(const void *h1, const void *h2){
	const struct rockstar_halo *elem1 = h1;
	const struct rockstar_halo *elem2 = h2;

	if (elem1->m < elem2->m)
		return -1;
	else if (elem1->m > elem2->m)
		return 1;
	else
		return 0;
}

int compare_id(const void *h1, const void *h2){
	const struct rockstar_halo *elem1 = h1;
	const struct rockstar_halo *elem2 = h2;

	if (elem1->id < elem2->id)
		return -1;
	else if (elem1->id > elem2->id)
		return 1;
	else
		return 0;
}
