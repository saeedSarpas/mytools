/*
 * grid/row_major_roder/one_to_three.c
 * test_file: grid/row_major_roder/one_to_three_test.c
 *
 * Converting a row-major index to the 3D indices
 *
 * param: index a row-major index
 * param: dims array containing the dimensions of the box
 *
 * TODO: making this function generic
 *
 * author: Saeed Sarpas
 */


#include <math.h>
#include "./../../memory/allocate.h"


int* one_to_three(int index, int *dims)
{
  index = index % (dims[0] * dims[1] * dims[2]);
  int *pos = allocate(3, sizeof(int));
  pos[0] = index / (dims[2] * dims[1]);
  index -= pos[0] * dims[2] * dims[1];
  pos[1] = index / dims[2];
  pos[2] = index - pos[1] * dims[2];

  return pos;
}
