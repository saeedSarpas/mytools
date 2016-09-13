#ifndef GADGET_DATA_TYPE_H
#define GADGET_DATA_TYPE_H

#include <inttypes.h>
#include "./../../avltree/avl_tree.h"

#define BINARY_HEADER_SIZE 256
#define VERSION_MAX_SIZE 12


typedef struct gadget_header
{
  int npart[6];
  double mass[6];
  double time;
  double redshift;
  int flag_sfr;
  int flag_feedback;
  int npartTotal[6];
  int flag_cooling;
  int num_files;
  double BoxSize;
  double Omega0;
  double OmegaLambda;
  double HubbleParam;
  char fill[256 - 6 * 4 - 6 * 8 - 2 * 8 - 2 * 4 - 6 * 4 - 2 * 4 - 4 * 8];
} gadget_header;


typedef struct gadget {
  struct gadget_header *headers;
  struct avl_node *particles;
} gadget;


#endif /* GADGET_DATA_TYPE_H */
