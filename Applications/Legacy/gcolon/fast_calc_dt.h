// File: fast_calc_dt.h
//
// header for fast_calc_dt.c
//
//

#include <stdlib.h>
#include <stdio.h>

#define MAGIC_NUMBER	32767

void dt3d3_4_5(short *outimage,
			  int dimx,
			  int dimy,
			  int dimz);


/*void calc_dt(short *imgVolume, short *binaryVolume, 
  int dimx, int dimy, int dimz);*/
