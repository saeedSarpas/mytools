/*
 * examples/rg_mmatchfinder/triplematches/triplecascade.c
 *
 * Generating triple tree (A bound tree with three roots as the matching
 * halos of three runs and their internal matching halos, aka main progenitors)
 *
 * Parameters
 * matches_512_256, matches_1024_512: an avltree containing the cross mathces
 * matches_256, matches_512, matches_1024: array of avltrees containing
 * internal matches
 * length: the length of matches_xxxx arrays
 *
 * Return
 * an array of allocated vector
 *
 * NOTE that by sending NULL instead of matches_256, matches_512, matches_1024
 * the function will skip adding internal
 * NOTE that the last element of the array is the only NULL element of it
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <string.h>
#include "./triplecascade.h"
#include "./../mh_cascade/mh_cascade.h"
#include "./../../matchinghalos/load_mh.h"
#include "./../../../ll/ll.h"
#include "./../../../avltree/avl_find.h"
#include "./../../../vector/vector_push.h"
#include "./../../../vector/vector_get.h"
#include "./../../../memory/allocate.h"
#include "./../../../configfile/mylibconfig.h"
#include "./../../../strings/concat/concat.h"
#include "./../../../io/check_file.h"
#include "./../../../time/mytime.h"
#include "./../../../halomatcher/singlehalo_matcher.h"


vector** triplecascade(avltree *matches_512_256,
                       avltree *matches_1024_512,
                       int nhalos,
                       avltree **matches_256,
                       avltree **matches_512,
                       avltree **matches_1024,
                       int length)
{
  vector **triplecascades = allocate(nhalos + 1, sizeof(*triplecascades));

  int haloid256, haloid512, haloid1024;
  avlnode *node_512_256, *node_1024_512;

  int i;
  for(i = 0; i < nhalos; i++){
    triplecascades[i] = new_vector(3, sizeof(ll*));
    haloid256 = i;

    // 256
    vector_push(triplecascades[i], mh_cascade(haloid256, matches_256, length));

    // 256 -> 512
    node_512_256 = avl_find(matches_512_256, &haloid256);

    if(node_512_256 == NULL)
        continue;

    haloid512 = ((match*)node_512_256->data)->matchid;

    // 512
    vector_push(triplecascades[i], mh_cascade(haloid512, matches_512, length));

    // 512 -> 1024
    node_1024_512 = avl_find(matches_1024_512, &haloid512);

    if(node_1024_512 == NULL)
      continue;

    haloid1024 = ((match*)node_1024_512->data)->matchid;

    //1024
    vector_push(triplecascades[i], mh_cascade(haloid1024, matches_1024, length));
  }

  triplecascades[nhalos] = NULL;

  return triplecascades;
}


void dispose_triplecascade(vector ***cascade)
{
  unsigned int i = 0, j;
  ll **pointer_to_list = NULL;
  vector *pointer_to_vector = NULL;

  while((*cascade)[i]){
    for(j = 0; j < (*cascade)[i]->logLength; j++){
      pointer_to_list = vector_get((*cascade)[i], j);
      dispose_ll(pointer_to_list);

      pointer_to_vector = (*cascade)[i];
      dispose_vector(&pointer_to_vector);
    }
    i++;
  }

  free(*cascade);
  *cascade = NULL;
}
