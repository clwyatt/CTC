// File: elongation.cc
// Abstract: calculate a measure of elongation of an object
//
// Created: 01/14/99 by: Chris L. Wyatt
// Modified:
//
//                
#include "elongation.h"

float elongation1(bframe3d * input)
{
   int i, j, k, num_surf = 0, num_vol = 0;
   int xdim, ydim, zdim;
   float ratio;

   input->querydim(&xdim, &ydim, &zdim); 

   for(i = 0; i < xdim; i++)
     for(j = 0; j < ydim; j++)
       for(k = 0; k < zdim; k++)
        {
	  if( input->getpixel(i, j, k) == TRUE ){
	    if( is_on_surface(input, i, j, k) == 1 ) num_surf += 1;
	    num_vol += 1;
	  }

	}

   ratio = ((float)(num_surf)) / ((float)(num_vol));

   return ratio;
}
