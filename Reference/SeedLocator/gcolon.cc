// File: gcolon.cc
// Abstract: Automate segmentation of the colon by
//           1) Growing Background region
//           2) Thresholding and merging with background
//           3) Computing Distance transform
//           4) Locating max distance.......
//
// Created: 07/13/98 by: Chris L. Wyatt
// Last Modified: 11/29/98 clw
//                  
//

#include "gcolon.h"


/*
* main routine
*/

int main(int argv, char **argc)
{
   char filename[100];
   char tempfilename[100];
   int xdim, ydim, zdim, tol, loopAgain;
   int i, j, k, sx, sy, sz;
   unsigned short * data;
   unsigned short * distdata;
   unsigned short max;
   bframe3d * region, * temp_region = NULL;
   bframe3d * colon, * temp_colon = NULL;
   //surface_point * surface, * current;
   //int numpnts;
   //double plane;
   FILE * seedf;

   if(argv < 2)
    {
      fprintf(stderr, "Usage: gcolon file [xdim ydim zdim] \n");
      fprintf(stderr, "\nwhere  file - name of volume file \n");
      fprintf(stderr, "       xdim - horizontal dimension\n");
      fprintf(stderr, "       ydim - vertical dimension\n");
      fprintf(stderr, "       zdim - z dimension (number of slices)\n\n");
      exit(1);
    }

   //parse command line, no checking!
   strcpy(filename, *(argc+1));
   //xdim = atoi(*(argc+2));
   //ydim = atoi(*(argc+3));
   //zdim = atoi(*(argc+4));

   //read in image from file
   //data = read_img_file(filename, xdim, ydim, zdim, 0);
   //data = read_dcm_file(filename, xdim, ydim, zdim, 1411);
   data = read_rvf_file(filename, &xdim, &ydim, &zdim);

   fprintf(stderr, "File %s read.\n", filename);

   //set tolerance on region grower threshold
   tol = 224;

   //segment background
   region = grow_region(0, 0, 0, xdim, ydim, zdim, tol, data);
   temp_region = grow_region(xdim-1, ydim-1, zdim-1, xdim, ydim, zdim, tol, data);
   for(i=0; i < xdim; i++)
	for(j=0; j < ydim; j++)
		for(k=0; k < zdim; k++)
		{
			if(temp_region->getpixel(i, j, k) == TRUE) 
				region->setpixel(i, j, k, TRUE);
		}
   delete temp_region;
   fprintf(stderr, "Background segmentation done.\n");

   distdata = new unsigned short[xdim*ydim*zdim];

   // prepare the image for 3-4-5 distance transform
   tol = 100;
   for(i=0; i < xdim; i++)
     for(j=0; j < ydim; j++)
       for(k=0; k < zdim; k++)
         {
           if( region->getpixel(i, j, k) == TRUE )
             *(distdata + k*xdim*ydim + i*ydim + j) = 0;
           else if(*(data + k*xdim*ydim + i*ydim + j) > tol)
	    *(distdata + k*xdim*ydim + i*ydim + j) = 0;
           else
             *(distdata + k*xdim*ydim + i*ydim + j) = MAGIC_NUMBER;
         }

   //compute distance transform
   fprintf(stderr, "Computing distance transform ......");
   dt3d3_4_5((short *)distdata, ydim, xdim, zdim);
   fprintf(stderr, "Done.\n");

   //locate maximum from distance transform
   max = *data;
   sx = 0; sy = 0; sz = 0;
   for(i=0; i < xdim; i++)
     for(j=0; j < ydim; j++)
       for(k=0; k < zdim; k++)
         {
          if( (*(distdata + k*xdim*ydim + i*ydim + j) > max) && 
              (*(distdata + k*xdim*ydim + i*ydim + j) != MAGIC_NUMBER) )
	   {
             max = *(distdata + k*xdim*ydim + i*ydim + j);
             sx = i; sy = j; sz = k;
           }
         }

   //print coordinates of seed
   printf("Seed is at (%i,%i,%i)\n", sx, sy, sz);

   //grow colon
   tol = 224;
   colon = grow_region(sx, sy, sz, xdim, ydim, zdim, tol, data);

//     sprintf(tempfilename, "%s.binary1", filename);
//     colon->write(tempfilename);

   //open seed file
   sprintf(tempfilename, "%s.seed", filename);
   seedf = fopen(tempfilename, "w");

   //write first seed
   fprintf(seedf, "%i %i %i\n", sx, sy, sz);

   // begin loop here
   loopAgain = 1;

   while(loopAgain){


     // mask previously grown region from distance transform data
     for(i=0; i < xdim; i++)
       for(j=0; j < ydim; j++)
	 for(k=0; k < zdim; k++)
         {
           if( (region->getpixel(i, j, k) == TRUE) || (colon->getpixel(i, j, k) == TRUE) )
             *(distdata + k*xdim*ydim + i*ydim + j) =  MAGIC_NUMBER;
         }

//  FILE *of;
//  of = fopen("binary.raw", "w");
//  fwrite((char *)(distdata), xdim*ydim*zdim*2, 1, of);
//  fclose(of);

     //locate maximum from distance transform
     max = 0;
     sx = 0; sy = 0; sz = 0;
     for(i=0; i < xdim; i++)
       for(j=0; j < ydim; j++)
	 for(k=0; k < zdim; k++)
         {
	   if( (*(distdata + k*xdim*ydim + i*ydim + j) > max) && 
              (*(distdata + k*xdim*ydim + i*ydim + j) != MAGIC_NUMBER) )
	   {
             max = *(distdata + k*xdim*ydim + i*ydim + j);
             sx = i; sy = j; sz = k;
           }
         }

     fprintf(stderr, "Max = %i\n", max);

     if(max > 10){
       //print coordinates of seed
       printf("Seed is at (%i,%i,%i)\n", sx, sy, sz);

       //write seed
       fprintf(seedf, "%i %i %i\n", sx, sy, sz);

       delete temp_colon;
       //set tolerance on region grower threshold
       tol = 224;
       //grow colon
       temp_colon = grow_region(sx, sy, sz, xdim, ydim, zdim, tol, data);

       // add new region to colon
       for(i=0; i < xdim; i++)
	 for(j=0; j < ydim; j++)
	   for(k=0; k < zdim; k++)
	   {
	     if(temp_colon->getpixel(i, j, k) == TRUE) colon->setpixel(i, j, k, TRUE);
	   }
       loopAgain = 1;
     }
     else
       loopAgain = 0;
   }

   // close seed file
   fclose(seedf);

   sprintf(tempfilename, "%s.final", filename);
   colon->write(tempfilename);

   //de-allocate memory
   delete region;
   delete colon;
   delete [] data;
   delete [] distdata;


}


//    FILE *of;
//    of = fopen("binary.raw", "w");
//    fwrite((char *)(distdata), xdim*ydim*zdim*2, 1, of);
//    fclose(of);
