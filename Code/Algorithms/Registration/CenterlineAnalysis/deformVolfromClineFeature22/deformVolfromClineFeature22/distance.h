#ifndef DISTANCE_H
#define DISTANCE_H

#include "tnt/tnt_array3d.h"

using namespace std;

template <class T>
void distance(TNT::Array3D<T> &volume)
{
  T mask[13] = {5, 4, 5, 4, 3, 4, 5, 4, 5, 4, 3, 4, 3};
  int dimx = volume.dim1();
  int dimy = volume.dim2();
  int dimz = volume.dim3();
  int i,j,k;

  T magic_number = dimx + dimy + dimz;

  /* Replace binary 1's with a value greater than the
     maximum possible distance in the volume */
  for ( k = 0; k < dimz; k++) {
    for ( j = 0; j < dimy; j++) {
      for ( i = 0; i < dimx; i++) {
	if (volume[i][j][k] == 1)
	  volume[i][j][k] = magic_number;
      }
    }
  }

  T sum = 0;

  for ( k = 1; k < dimz-1; k++) {                    /* Forward Pass */
    for ( j = 1; j < dimy-1; j++) {
      for ( i = 1; i < dimx-1; i++) {        	/* Mask Position */
	
	sum = volume[i-1][j-1][k-1] + mask[0];		/* -1 -1 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j-1][k-1] + mask[1];		/*  0 -1 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j-1][k-1] + mask[2];		/*  1 -1 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j][k-1] + mask[3];		/* -1  0 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j][k-1] + mask[4];		/*  0  0 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j][k-1] + mask[5];		/*  1  0 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j+1][k-1] + mask[6];		/* -1  1 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j+1][k-1] + mask[7];		/*  0  1 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j+1][k-1] + mask[8];		/*  1  1 -1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j-1][k] + mask[9];		/* -1 -1  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j-1][k] + mask[10];		/*  0 -1  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j-1][k] + mask[11];		/*  1 -1  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j][k] + mask[12];		/* -1  0  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
      }
    }
  }

  for ( k = dimz-2; k > 0; k--) {                    /* Backward Pass */
    for ( j = dimy-2; j > 0; j--) {
      for ( i = dimx-2; i > 0; i--) {		/* Mask Position */
	
	sum = volume[i+1][j+1][k+1] + mask[0];		/*  1  1  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j+1][k+1] + mask[1];		/*  0  1  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j+1][k+1] + mask[2];		/* -1  1  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j][k+1] + mask[3];		/*  1  0  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j][k+1] + mask[4];		/*  0  0  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j][k+1] + mask[5];		/* -1  0  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j-1][k+1] + mask[6];		/*  1 -1  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j-1][k+1] + mask[7];		/*  0 -1  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j-1][k+1] + mask[8];		/* -1 -1  1 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j+1][k] + mask[9];		/*  1  1  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i][j+1][k] + mask[10];		/*  0  1  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i-1][j+1][k] + mask[11];		/* -1  1  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
	
	sum = volume[i+1][j][k] + mask[12];		/*  1  0  0 */
	if (sum < volume[i][j][k])
	  volume[i][j][k] = sum;
      }
    }
  }
}

#endif
