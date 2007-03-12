// File: dilate.cc
// Abstract: dilate 3D 6-connected region
//
// Created: 07/30/98 by: Chris L. Wyatt
// Modified: 
//
//                
#include "dilate.h"

bframe3d * dilate(bframe3d * input)
{
   bframe3d * output;
   int dx, dy, dz;

   input->querydim(&dx, &dy, &dz);

   output = new bframe3d;
   output->setdim(dx, dy, dz);
   output->clear();

   for(int i=0; i < dx; i++)
     for(int j=0; j < dy; j++)
       for(int k=0; k < dz; k++)
	 {
          if( input->getpixel(i, j, k) == TRUE )
	    {
              output->setpixel(i, j, k, TRUE);
              output->setpixel(i+1, j, k, TRUE);
              output->setpixel(i-1, j, k, TRUE);
//               output->setpixel(i, j+1, k, TRUE);
//               output->setpixel(i, j-1, k, TRUE);
//               output->setpixel(i, j, k+1, TRUE);
//               output->setpixel(i, j, k-1, TRUE);
            }
         }

  return output;
}
