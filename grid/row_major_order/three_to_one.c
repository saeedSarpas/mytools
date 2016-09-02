/*
 * grid/row_major_roder/three_to_one.c
 * test_file: grid/row_major_roder/three_to_one_test.c
 *
 * Converting the index of an element in a 3D array to its row-major counter
 * index part. Note that this function assume the box is periodic so index -1
 * is actually the last grid in the same axis
 *
 * param: i, j, k indices of the element in the 3D array
 * param: dims array containing the dimensions of the box
 *
 * TODO: making this function generic
 *
 * author: Saeed Sarpas
 */


#include <math.h>


static int boundry_check(int, int);


int three_to_one(int i, int j, int k, int *dims)
{
  i = boundry_check(i, dims[0]);
	j = boundry_check(j, dims[1]);
	k = boundry_check(k, dims[2]);

	return (i * dims[2] * dims[1]) + (j * dims[2]) + k;
}


static int boundry_check(int pos, int axis_length)
{
  if(pos >= 0 && pos <= axis_length - 1)
    return pos;
  else
    if (pos > axis_length - 1)
      pos %= axis_length;
    else if (pos < 0)
      pos += axis_length;

  return boundry_check(pos, axis_length);
}
