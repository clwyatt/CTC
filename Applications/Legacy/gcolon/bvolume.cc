// File: bvolume.cc
// Created: 7/10/96 by: Chris L. Wyatt
// Modified: 07/28/98 from imageclass.cc to use only binary class
//
//                  

#include "bvolume.h"

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageFileWriter.h"

using namespace std;

//************************************
// bframe3d methods
//************************************


bframe3d::bframe3d() // default constructor
{
   horz_res = 512;  // default resolution
   vert_res = 512;
   z_res = 60;
   frame_data = new bool[horz_res*vert_res*z_res]; // data array creation
}

bframe3d::bframe3d(int horz_res, int vert_res, int z_res)  // constructor
{
   if ((horz_res <= 0 )||(vert_res <= 0))
     {
       // set error condition
     }
   //fprintf(stderr, "bframe3d constructor called: dims= %i, %i, %i\n",
                                                //horz_res, vert_res, z_res);
   frame_data = new bool[horz_res*vert_res*z_res];
}

bframe3d::bframe3d(const bframe3d & in)  // copy constructor
{
   int x, y, z;
   bool temp;
   fprintf(stderr, "Copy Constructor Called!!\n");

   if ((in.horz_res <= 0)||(in.vert_res <= 0))
    {
     // set error condition
    }
   horz_res = in.horz_res;
   vert_res = in.vert_res;
   z_res = in.z_res;

   frame_data = new bool[horz_res*vert_res*z_res];
   for(z=0; z < z_res; z++)
    for(x=0; x < horz_res; x++)
     for(y=0; y < vert_res; y++){
      temp = in.frame_data[x*vert_res + y + z*horz_res*vert_res];
      frame_data[x*vert_res + y + z*horz_res*vert_res]= temp;
      }

}

bframe3d::~bframe3d()  // destructor
{
   //fprintf(stderr, "bframe3d destructor called!!!!!!\n");
   delete [] frame_data;
}

void bframe3d::setpixel(int x, int y, int z, bool value)
{
  //fprintf(stderr, "Current dims: %i,%i,%i\n", horz_res, vert_res, z_res);
  //fprintf(stderr, "pointer calc= %i,%i,%i\n", x, y, z);

 if((x<horz_res)&(y<vert_res)&(z<z_res))
  (*(frame_data + z*horz_res*vert_res + x*vert_res + y))= value;
}

bool bframe3d::getpixel(int x, int y, int z)
{

  bool value;

  //if((x>horz_res)||(y>vert_res)||(z>z_res))
//fprintf(stderr, "Seg fault coming: %i, %i, %i were requested dims\n", x, y, z);

 if((x<horz_res)&(y<vert_res)&(z<z_res))
   value= frame_data[z*horz_res*vert_res + x*vert_res + y];
 else
   value= FALSE;

   return value;
}

void bframe3d::querydim(int *x, int *y, int *z)  //query image dims
{

   *x = horz_res;
   *y = vert_res;
   *z = z_res;
}

void bframe3d::setdim(int x, int y, int z)  //set 3d frame dims
                                      //NOTE: may delete data currently 
                                      //      in stack!
{

   if((x>horz_res)||(y>vert_res)||(z>z_res))
    {
      //need to allocate more memory
      //fprintf(stderr, "Increasing size of 3D frame stack to %i\n", z);
      delete frame_data;
      horz_res = x;
      vert_res = y;
      z_res = z;
      frame_data = new bool[x*y*z];
    }
   else
    {
      //just use what is available and reset dims
      //Note: no data lost;
      horz_res= x;
      vert_res= y;
      z_res= z;
     }
}

bframe3d & bframe3d::operator=(const bframe3d & in)
{
   int x, y;

   fprintf(stderr, "in bframe3d: operator= overload called!\n");

   if ((in.horz_res <= 0)||(in.vert_res <= 0))
    {
     // set error condition
    }
   horz_res = in.horz_res;
   vert_res = in.vert_res;
   delete frame_data; //inefficient but easy!!!!!!!
   frame_data = new bool[horz_res*vert_res];
   
   for(x=0; x < in.horz_res; x++)
     for(y=0; y < in.vert_res; y++)
      (*(frame_data + x*vert_res + y)) = 
                             (*(in.frame_data + x*vert_res + y));

   return *this;
   //return temp;
}

void bframe3d::clear() // clear all to FALSE
{

  int i, j, k;

  for(k=0; k< z_res; k++)
    for(i=0; i < horz_res; i++)
      for(j=0; j < vert_res; j++)
        frame_data[k*horz_res*vert_res + j*horz_res + i]= FALSE;
}

// remove all inner points of region leaving surface
//                  #
// region model =  #X#
//                  #
void bframe3d::surface() // remove all inner points of region leaving surface
{

  int i, j, k;
  bool flg;
  bool *data, *data1, *data2;
  data = new bool[horz_res*vert_res];
  data1 = new bool[horz_res*vert_res];
  data2 = new bool[horz_res*vert_res];

  for(k=0; k < z_res; k++)
   {
    //make copy of current frame
     for(i=0; i < horz_res; i++)
       for(j=0; j < vert_res; j++)
        data[j*horz_res + i]= frame_data[k*horz_res*vert_res + j*horz_res + i];

     for(j=0; j < vert_res; j++)
      {
        flg= FALSE;
        for(i=0; i < horz_res; i++)
         {
          if((data[j*horz_res + i]) & (!flg))
           {
            flg= TRUE;
            data1[j*horz_res + i]= TRUE;
           }
          else if((data[j*horz_res + i]) & (flg))
            data1[j*horz_res + i]= FALSE;
          else if((!data[j*horz_res + i]) & (flg))
            {
             flg= FALSE;
             data1[j*horz_res + (i-1)]= TRUE;
            }
          else
            data1[j*horz_res + i]= FALSE; 
         }
      }

     for(i=0; i < vert_res; i++)
      {
        flg= FALSE;
        for(j=0; j < horz_res; j++)
         {
          if((data[j*horz_res + i]) & (!flg))
           {
            flg= TRUE;
            data2[j*horz_res + i]= TRUE;
           }
          else if((data[j*horz_res + i]) & (flg))
            data2[j*horz_res + i]= FALSE;
          else if((!data[j*horz_res + i]) & (flg))
            {
             flg= FALSE;
             data2[j*horz_res + (i-1)]= TRUE;
            }
          else
            data2[j*horz_res + i]= FALSE;
         }
      }
    for(i=0; i < horz_res; i++)
     for(j=0; j < vert_res; j++)
      {
       if((data1[j*horz_res + i]) || (data2[j*horz_res + i]))
         frame_data[k*horz_res*vert_res + j*horz_res + i]= TRUE;
       else
         frame_data[k*horz_res*vert_res + j*horz_res + i]= FALSE;
      }
   }
   delete data;
   delete data1;
   delete data2;
}


float * bframe3d::vert(int *maxverts) //generate verticies for render
{

  int i, j, k, count;
  float *vert;

  //determine number of points on surface to allocate memory
  count = 0;
  for(k=0; k < z_res; k++)
   for(i=0; i < horz_res; i++)
     for(j=0; j < vert_res; j++)
       if(frame_data[k*horz_res*vert_res + j*horz_res + i])
        count += 1;

  //allocate memory
  vert = new float[3*count];
  (*maxverts)= count;

  fprintf(stderr, "In vert() maxverts= %i\n", (*maxverts));

  //load points into the array
  count = 0;
  for(k=0; k < z_res; k++)
   for(i=0; i < horz_res; i++)
     for(j=0; j < vert_res; j++)
       if(frame_data[k*horz_res*vert_res + j*horz_res + i])
         {
          //map points into 2x2x2 cube ???????
          vert[count + 0]= (float(i)/128)-1;  //put in x coord
          vert[count + 1]= (float(j)/128)-1;  //put in y coord
          vert[count + 2]= (float(k)/30)-1;  //put in z coord

          count += 3;
         }

  //Remember memory has been allocated!! 
  //if I call this function again I'm going to need a flag to delete/realloc

  return vert;
}

float * bframe3d::norm(int *maxverts) //generate normals for render
{

  int i, j, k, count, a;
  int x1, y1, x2, y2;
  float *norm;
  float normx, normy, normz, dx, dy, mag;

  //determine number of points on surface to allocate memory
  count = 0;
  for(k=0; k < z_res; k++)
   for(i=0; i < horz_res; i++)
     for(j=0; j < vert_res; j++)
       if(frame_data[k*horz_res*vert_res + j*horz_res + i])
        count += 1;

  fprintf(stderr, "In norm() maxverts= %i count = %i\n", (*maxverts), count);

  //check if dims match, if not fatal error
  if(count != (*maxverts))
   {
     fprintf(stderr, "Error: Diminsions do not match in bframe3d::norm\n");
     fprintf(stderr, "Error: count = %i, maxverts= %i\n", count, maxverts);
     exit(1);
   }

  //allocate memory
  norm = new float[3*count];

  //calculate and load normals into the array
  count = 0;
  for(k=0; k < z_res; k++)
   for(i=0; i < horz_res; i++)
     for(j=0; j < vert_res; j++)
       if(frame_data[k*horz_res*vert_res + j*horz_res + i])
         {
          a = 0;
          //find coordinates of adjoining pixels
          if(frame_data[k*horz_res*vert_res + (j-1)*horz_res + i])
            {
              x1= i;
              y1= j-1;
              a=a+1;
            }
          if(frame_data[k*horz_res*vert_res + (j+1)*horz_res + i])
            {
              if(a == 0){
               x1= i;
               y1= j+1;}
              else{
               x2= i;
               y2= j+1;}
              a=a+1;
            }
          if((frame_data[k*horz_res*vert_res + j*horz_res + (i-1)])&(a<2))
            {
              if(a == 0){
               x1= i-1;
               y1= j;}
              else{
               x2= i-1;
               y2= j;}
              a=a+1;
            }
          if((frame_data[k*horz_res*vert_res + j*horz_res + (i+1)])&(a<2))
            {
               if(a == 0){
               x1= i+1;
               y1= j;}
              else{
               x2= i+1;
               y2= j;}
              a=a+1;
            }
          if((frame_data[k*horz_res*vert_res + (j-1)*horz_res + (i-1)])&(a<2))
            {
              if(a == 0){
               x1= i-1;
               y1= j-1;}
              else{
               x2= i-1;
               y2= j-1;}
              a=a+1;
            }
          if((frame_data[k*horz_res*vert_res + (j+1)*horz_res + (i-1)])&(a<2))
            {
              if(a == 0){
               x1= i-1;
               y1= j+1;}
              else{
               x2= i-1;
               y2= j+1;}
              a=a+1;
            }
          if((frame_data[k*horz_res*vert_res + (j-1)*horz_res + (i+1)])&(a<2))
            {
              if(a == 0){
               x1= i+1;
               y1= j-1;}
              else{
               x2= i+1;
               y2= j-1;}
              a=a+1;
            }
          if((frame_data[k*horz_res*vert_res + (j+1)*horz_res + (i+1)])&(a<2))
            {
              if(a == 0){
               x1= i+1;
               y1= j+1;}
              else{
               x2= i+1;
               y2= j+1;}
              a=a+1;
            }

          //calculate partial with respect to x
          dx= (float(y2) - float(y1))/2;
          //dx= 1.0;
          //calculate partial with respect to y
          dy= (float(x1) - float(x2))/2;
          //dy= 1.0;
          //calculate partial with respect to z
          //dz=TEMPORARY!

          //Normalize
          mag= sqrt(dx*dx + dy*dy);
          if(a<2)
           {
             fprintf(stderr, "Error in norm: no connected pixel\n");
             dx= 0.0;
             dy= 0.0;
             mag= 1.0;
           }
          if(mag == 0){
             fprintf(stderr, "Mag= 0 @ x1=%f, y1=%f, x2=%f, y2=%f\n", float(x1),
                                                                     float(y1), 
                                                                     float(x2),
                                                                     float(y2));
             fprintf(stderr, "         dx=%f, dy=%f\n", dx, dy);
             fprintf(stderr, "         i=%i, j=%i, k=%i\n", i, j, k);}
  
          normx= dx/mag;
          normy= dy/mag;
          normz= 0.0;   //temporary!

          norm[count + 0]= normx;  //put in x coord
          norm[count + 1]= normy;  //put in y coord
          norm[count + 2]= normz;  //put in z coord
          count += 3;
         }

  //Remember memory has been allocated!! 
  //if I call this function again I'm going to need a flag to delete/re-alloc

  return norm;
}


float * bframe3d::spline(int * np)
{

  int i, j, k, num, ii, last;
  bool first= FALSE;
  int xdim, ydim, zdim;
  float * sp;

  xdim= horz_res;
  ydim= vert_res;
  zdim= z_res;
  num= 0;

  //first find number of points to alloc memory

  for(i=0; i < xdim; i++){
    first= FALSE;
    for(k=0; k < zdim; k++)
      for(j=0; j < ydim; j++){
        if(frame_data[k*xdim*ydim + i*xdim + j] & !first){
           first= TRUE;
           last=j;
        }
        if(frame_data[k*xdim*ydim + i*xdim + j] & ((abs(last-j))< 50)){
          last=j;
          num=num+1;
          break;
        }
      }
  }
  (*np)= num;

  fprintf(stderr, "There are %i spline points.\n", num);

  //allocate the memory (need a test flag to delete/reallocate!!)
  sp= new float[3*num];

  //repeat loop putting points
  ii= 0;
  first= FALSE;
  for(i=0; i < xdim; i++){
    first=FALSE;
    for(k=0; k < zdim; k++)
      for(j=0; j < ydim; j++){
        if(frame_data[k*xdim*ydim + i*xdim + j] & !first){
           first= TRUE;
           last=j;
        }
        if(frame_data[k*xdim*ydim + i*xdim + j] & ((abs(last-j))< 50)){
          (*(sp+ii))= (float(i)/128)-1;
          (*(sp+ii+1))= (float(j)/128)-1;
          (*(sp+ii+2))= (float(k)/30)-1;
          ii= ii+3;
          last=j;
          break;
        }
      }
  }

  return sp;
}

int bframe3d::getvert(int i, int j, int k, unsigned int v)
{

  int value;
  int x, y, z;
  bool temp;

  if((i< 0) || (i> horz_res) || (j<0) || (j> vert_res) || (k<0) || (k> z_res))
   {
    fprintf(stderr, "Error: index out of bounds in  bframe3d::getvert\n");
    fprintf(stderr, "Error: requested index: %i, %i, %i\n", i, j, k);
    fprintf(stderr, "Error: current resolution %i x %i x %i\n", horz_res,
                                                 vert_res, z_res);
    exit(1);
   }

  x=i; y=j; z=k;
  if((v & 1)==1) x+=1;
  if((v & 2)==2) y+=1;
  if((v & 4)==4) z+=1;

  temp= frame_data[z*horz_res*vert_res + x*horz_res + y];

  if(temp)
    value= -1;
  else
    value= 1;

  //if( value == -1 ){
   // fprintf(stderr, "i=%i, j=%i, k=%i, x=%i, y=%i, z=%i  ", i, j, k, x, y, z);
   // fprintf(stderr, "   V=%i", v);
   // fprintf(stderr, "   value=%i\n", value);}

  return value;
}

   
 
void bframe3d::write(char * filename)
{
   ofstream outfile;
   unsigned char * data;

   data = new unsigned char[horz_res*vert_res*z_res];

   for(int i=0; i < horz_res; i++)
     for(int j=0; j < vert_res; j++)
       for(int k=0; k < z_res; k++)
	 {
          if( this->getpixel(i, j, k) == TRUE )
            *(data + k*horz_res*vert_res + i*vert_res + j) = 0;
          else
            *(data + k*horz_res*vert_res + i*vert_res + j) = 255;
         }

   typedef itk::Image< unsigned char, 3 > ImageType;
   typedef itk::ImportImageFilter< unsigned char, 3 > ImportFilterType;

   ImportFilterType::Pointer importFilter = ImportFilterType::New();

   ImportFilterType::SizeType size;
   size[0] = horz_res; // size along X
   size[1] = vert_res; // size along Y
   size[2] = z_res; // size along Z
   unsigned int numberOfPixels = size[0]*size[1]*size[2];

   ImportFilterType::IndexType start;
   start.Fill( 0 );
   ImportFilterType::RegionType region;
   region.SetIndex( start );
   region.SetSize( size );
   importFilter->SetRegion( region );
   
   double origin[3];
   origin[0] = 0.0; // X coordinate
   origin[1] = 0.0; // Y coordinate
   origin[2] = 0.0; // Z coordinate
   importFilter->SetOrigin( origin );
   
   double spacing[3];
   spacing[0] = 1.0; // along X direction
   spacing[1] = 1.0; // along Y direction
   spacing[2] = 1.0; // along Z direction
   importFilter->SetSpacing( spacing );
   
   importFilter->SetImportPointer( data, numberOfPixels, true );

   typedef itk::ImageFileWriter< ImageType > WriterType;
   WriterType::Pointer writer = WriterType::New();
   writer->SetInput(importFilter->GetOutput());
   writer->SetFileName(filename);
   writer->Update();

   //outfile.open( filename );
   //outfile.write((char *)data, horz_res*vert_res*z_res);
   //outfile.close();
   
   //delete [] data;
}


void bframe3d::read(char * filename, int xdim, int ydim, int zdim)
{
   ifstream infile;
   unsigned char * data;

   data = new unsigned char[xdim*ydim*zdim];

   this->setdim(xdim, ydim, zdim);

   infile.open( filename );
   infile.read((char *)data, horz_res*vert_res*z_res);
   infile.close();

   for(int i=0; i < horz_res; i++)
     for(int j=0; j < vert_res; j++)
       for(int k=0; k < z_res; k++)
	 {
          if( *(data + k*horz_res*vert_res + i*vert_res + j) != 0 )
            this->setpixel(i, j, k, TRUE);
          else
            this->setpixel(i, j, k, FALSE);
         }

  delete [] data;
}

