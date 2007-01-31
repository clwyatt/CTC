#ifndef LOCAL_MAXIMA_H
#define LOCAL_MAXIMA_H

#include <vector>
#include "tnt/tnt_array3d.h"

using namespace std;

/* 3,4,5 Mask */
static int maskVector[3][3][3] = {{{5,4,5},{4,3,4},{5,4,5}},
				  {{4,3,4},{3,0,3},{4,3,4}},
				  {{5,4,5},{4,3,4},{5,4,5}}};

/* Structure to hold the coordinates and radius associated with a local maximum point */
template <class Type>
struct localMaximum {
  Type x, y, z;
  Type radius;
};

/* function that generates a list of local maxima */
template <class T>
bool isLMPoint(const TNT::Array3D<T> &DT, const int i, const int j, const int k) {
  int x,y,z;
  for (z = -1; z < 2; z++) {
    for (y = -1; y < 2; y++) {
      for (x = -1; x < 2; x++) {
	/* ignore center mask voxel */
	if (!((x == 0) && (y == 0) && (z == 0))) {
//	  	mexPrintf("i=%d, j=%d, k=%d (x=%d,y=%d,z=%d): DT[+]=%f, add=%f DT[][][]=%f maskvector=%d\n",
//		i,j,k,x,y,z,DT[i+x][j+y][k+z],(DT[i][j][k] + maskVector[x+1][y+1][z+1]),DT[i][j][k],maskVector[x+1][y+1][z+1]);

	  /* Local Maximum Critereon (LMC) */
//	  if (DT[i+x][j+y][k+z] == (DT[i][j][k] + maskVector[x+1][y+1][z+1]))
	  if (DT[i+x][j+y][k+z] >= (DT[i][j][k] + maskVector[x+1][y+1][z+1])){  // by wook
//	 	mexPrintf("No LM  \n"); 	
	    return false;
	}
	}
      }
    }
  }
  return true;
}


template <class T>
void findLocalMaxima(const TNT::Array3D<T> &DT, vector< localMaximum<T> > &LMList)
{
  int dimx = DT.dim1();
  int dimy = DT.dim2();
  int dimz = DT.dim3();
  
  /* a temporary local maximum point to be added to the list when criterea is met */
  localMaximum<T> temp;
  
  for (int k = 0; k < dimz; k++) {
    for (int j = 0; j < dimy; j++) {
      for (int i = 0; i < dimx; i++) {
	if (DT[i][j][k] != 0) {
	  /* test LMC on a point */
	  if (isLMPoint(DT,i,j,k)) {
	    temp.x = i;
	    temp.y = j;
	    temp.z = k;
	    temp.radius = DT[i][j][k];
	    LMList.push_back(temp);
	  }
	}
      }
    }
  }
}

#endif
