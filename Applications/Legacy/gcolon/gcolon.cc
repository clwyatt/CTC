// File: gcolon.cc
// Abstract: Automate segmentation of the colon by
//           1) Growing Background region
//           2) Thresholding and merging with background
//           3) Computing Distance transform
//           4) Locating max distance.......
//
// Created: 07/13/98 by: Chris L. Wyatt
// Last Modified: 12/28/98 clw
//                  
//

#include "gcolon.h"

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageFileWriter.h"


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
   bframe3d * region, * temp_region = NULL, * fluid = NULL;
   bframe3d * colon, * temp_colon = NULL, * colon_flat = NULL, * colon_dil;
   surface_point * surface, * current;
   int numpnts;
   float region_size;
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
   tol = 224; //was 224

   //segment background
   region = grow_region(0, 0, 0, xdim, ydim, zdim, tol, data);

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

//    typedef itk::Image< unsigned short, 3 > ImageType;
//    typedef itk::ImportImageFilter< unsigned short, 3 > ImportFilterType;

//    ImportFilterType::Pointer importFilter = ImportFilterType::New();

//    ImportFilterType::SizeType size;
//    size[0] = xdim; // size along X
//    size[1] = ydim; // size along Y
//    size[2] = zdim; // size along Z
//    unsigned int numberOfPixels = size[0]*size[1]*size[2];

//    ImportFilterType::IndexType start;
//    start.Fill( 0 );
//    ImportFilterType::RegionType dregion;
//    dregion.SetIndex( start );
//    dregion.SetSize( size );
//    importFilter->SetRegion( dregion );
   
//    double origin[3];
//    origin[0] = 0.0; // X coordinate
//    origin[1] = 0.0; // Y coordinate
//    origin[2] = 0.0; // Z coordinate
//    importFilter->SetOrigin( origin );
   
//    double spacing[3];
//    spacing[0] = 1.0; // along X direction
//    spacing[1] = 1.0; // along Y direction
//    spacing[2] = 1.0; // along Z direction
//    importFilter->SetSpacing( spacing );
   
//    importFilter->SetImportPointer( data, numberOfPixels, true );

//    typedef itk::ImageFileWriter< ImageType > WriterType;
//    WriterType::Pointer writer = WriterType::New();
//    writer->SetInput(importFilter->GetOutput());
//    writer->SetFileName("dt.vtk");
//    writer->Update();

   colon = new bframe3d;
   colon->setdim(xdim, ydim, zdim);
   colon->clear();


   //open seed file
   sprintf(tempfilename, "%s.seed", filename);
   seedf = fopen(tempfilename, "w");

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
//   of = fopen("binary.raw", "w");
//   fwrite((char *)(distdata), xdim*ydim*zdim*2, 1, of);
//   fclose(of);

//   return 0;

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

     if(max > 50){

       //print coordinates of seed
       printf("Seed is at (%i,%i,%i)\n", sx, sy, sz);

       //write seed
       fprintf(seedf, "%i %i %i\n", sx, sy, sz);

       //set tolerance on region grower threshold
       tol = 224; //was 200
       //grow colon
       temp_colon = grow_region(sx, sy, sz, xdim, ydim, zdim, tol, data);

       temp_colon->write("temp_colon.vtk");

       //compute curvature
       fprintf(stderr, "Computing curvature.\n");
       surface = curvature(temp_colon, &numpnts);

       //delete colon_flat;
       colon_flat = new bframe3d;
       colon_flat->setdim(xdim, ydim, zdim);
       colon_flat->clear();

       current = surface;
       // was 0.2 for decimated volumes
       // was 0.01 for full size volumes 
       for(i = 0; i < numpnts; i++){
         if( (fabs(current->K) < 0.01) && (fabs(current->H) < 0.01) ){
           colon_flat->setpixel(current->x, current->y, current->z, TRUE);
         }
         current = current->next;
       }

      fprintf(stderr, "Selecting planar areas.\n");
      select_area(colon_flat, 150); //75 for decimated volumes
                                   //150 for full size volumes

      colon_flat->write("flat.vtk");

      fprintf(stderr, "Growing across pv\n");
      colon_dil = grow_region_pv(colon_flat, 0.1, data); //was 0.05
      delete colon_flat;

      //colon_dil->write("/home/users/cwyatt/data/temp.dil.raw");

      fprintf(stderr, "Growing Gradient Constrained Region.\n");
      tol = 15; //45 too high
      fluid = grow_region_grad3(colon_dil, temp_colon, tol, data, &region_size);
      delete colon_dil;

      fprintf(stderr, "Merging Colon and Fluid.\n");
      temp_region = temp_colon;
      temp_colon = merge(temp_region, fluid);
      delete fluid;

       // add new region to colon if not obviously too big.
       // else just add air segmentation

       if(region_size < ((float)(xdim*ydim*zdim))/32)
       { //add air plus fluid segmentation
         for(i=0; i < xdim; i++)
	   for(j=0; j < ydim; j++)
	     for(k=0; k < zdim; k++)
	     {
	       if(temp_colon->getpixel(i, j, k) == TRUE) colon->setpixel(i, j, k, TRUE);
	     }
       }
       else
       {
         fprintf(stderr, "DEBUG: fluid region too big, discarding....\n");
         //just add air segmentation
         for(i=0; i < xdim; i++)
	   for(j=0; j < ydim; j++)
	     for(k=0; k < zdim; k++)
	     {
	       if(temp_region->getpixel(i, j, k) == TRUE) colon->setpixel(i, j, k, TRUE);
	     }
       }
       delete temp_region;
       delete temp_colon;
       loopAgain = 0;
     }
     else
       loopAgain = 0;
   }

   // close seed file
   fclose(seedf);

   sprintf(tempfilename, "%s.vtk", filename);
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
