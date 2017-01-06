/*
 * examples/rg_mmatchfinder/triplematches/triplecascade.c
 *
 * Generating triple tree (A bound tree with three roots as the matching
 * halos of three runs and their internal matching halos, aka main progenitors)
 *
 * Parameters
 * matches_256_512, matches_512_1024: an avltree containing the cross mathces
 * matches_256, matches_512, matches_1024: array of avltrees containing
 * internal matches
 * length: the length of matches_xxxx arrays
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
#include "./../../../memory/allocate.h"
#include "./../../../configfile/mylibconfig.h"
#include "./../../../strings/concat/concat.h"
#include "./../../../io/check_file.h"
#include "./../../../time/mytime.h"


vector** triplecascade(avltree *matches_256_512, avltree *matches_512_1024,
    avltree **matches_256, avltree **matches_512, avltree **matches_1024,
    int length)
{
  int nhalos_256 = *(int*)((max_node(matches_256_512->root))->key) + 1;
  vector **triplecascades = allocate(nhalos_256, sizeof(vector*));

  int haloid256, haloid512, haloid1024;
  avlnode *node_256_512, *node_512_1024;

  int i;
  for(i = 0; i < nhalos_256; i++){
    triplecascades[i] = new_vector(3, sizeof(ll*));
    haloid256 = i;

    // 256
    vector_push(triplecascades[i], mh_cascade(haloid256, matches_256, length));

    // 256 -> 512
    node_256_512 = avl_find(matches_256_512, &haloid256);

    if(node_256_512 == NULL)
        continue;

    haloid512 = *(int*)node_256_512->data;

    // 512
    vector_push(triplecascades[i], mh_cascade(haloid512, matches_512, length));

    // 512 -> 1024
    node_512_1024 = avl_find(matches_512_1024, &haloid512);

    if(node_512_1024 == NULL)
        continue;

    haloid1024 = *(int*)node_512_1024->data;

    //1024
    vector_push(triplecascades[i], mh_cascade(haloid1024, matches_1024, length));
  }

  return triplecascades;
}
