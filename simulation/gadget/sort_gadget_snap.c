/*
 * simulation/sort_gadget_snap.c
 * test_file: simulation/sort_gadget_snap_test.c
 *
 * Sorting gadget particles based on their different attributes
 *
 * param: g already filled/loaded gadget struct
 *
 * TODO: making this function generic
 *
 * author: Saeed Sarpas
 */


#include "sort_gadget_snap.h"


int compare_id(const void*, const void*);


void sort_gadget_snap(struct gadget *g){
  int i, offset = 0;
  for(i = 0; i < 6; i++){
    if(g->headers->npart[i] != 0)
      qsort((void*)&g->particles[offset], g->headers->npart[i],
            sizeof(struct particle), compare_id);
    offset += g->headers->npart[i];
  }
}


int compare_id(const void *h1, const void *h2){
	const struct particle *elem1 = h1;
	const struct particle *elem2 = h2;

	if (elem1->id < elem2->id)
		return -1;
	else if (elem1->id > elem2->id)
		return 1;
	else
		return 0;
}
