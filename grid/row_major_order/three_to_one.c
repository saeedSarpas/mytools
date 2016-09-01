/*
 * grid/row_major_roder/three_to_one.c
 * test_file: grid/row_major_roder/three_to_one_test.c
 *
 * Converting the index of an element in a 3D array to its row-major
 * counter index
 *
 * param: i, j, k indices of the element in the 3D array
 * param: dims array containing the dimensions of the box
 *
 * TODO: making this function generic
 *
 * author: Saeed Sarpas
 */


#include <math.h>


int three_to_one(int i, int j, int k, int *dims)
{
	i = i % dims[0];
	j = j % dims[1];
	k = k % dims[2];

	return (i * dims[2] * dims[1]) + (j * dims[2]) + k;
}
