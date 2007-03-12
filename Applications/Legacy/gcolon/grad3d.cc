// File: grad3d.cc
// Abstract:
//
// Created: 07/23/98 by: Chris L. Wyatt
// Last Modified: 07/24/98 clw
//                  
//

#include "grad3d.h"


void conv3d(unsigned short *data, 
            int xdim, int ydim, int zdim,
            float * grad_mag)
{

   int i, j, k, l, m, n, index1, index2;
   float tempx, tempy, tempz;

   //zero top and bottom sides
   for(i = 0; i < xdim; i++)
     for(j = 0; j < ydim; j++)
       for(k = 0; k < zdim; k++)
       {
        *(grad_mag + k*xdim*ydim + i*ydim + j) = 0;
       }

   for(i = 1;i < (xdim-1); i++)
     for(j = 1;j < (ydim-1); j++)
       for(k = 1;k < (zdim-1); k++)
	 {
          tempx = 0;
          tempy = 0;
          tempz = 0;
          for(l = 0; l < 3; l++)
            for(m = 0; m < 3; m++)
              for(n = 0; n < 3; n++)
               {
                index1 = (k+l-1)*xdim*ydim + (i+m-1)*ydim + (j+n-1);
                index2 = l*9 + m*3 + n;
                tempx = tempx + (float)(data[index1]*dx[index2])*0.037037;
                tempy = tempy + (float)(data[index1]*dy[index2])*0.037037;
                tempz = tempz + (float)(data[index1]*dz[index2])*0.037037;
               }
          *(grad_mag + k*xdim*ydim + i*ydim + j) = sqrt(tempx*tempx + tempy*tempy + tempz*tempz);
         }
}


void xyang3d(unsigned short *data, 
            int xdim, int ydim, int zdim,
            float * xyangle)
{

   int i, j, k, l, m, n, index1, index2;
   float tempx, tempy, tempz;

   //zero top and bottom sides
   for(i = 0; i < xdim; i++)
     for(j = 0; j < ydim; j++)
       for(k = 0; k < zdim; k++)
       {
        *(xyangle + k*xdim*ydim + i*ydim + j) = 0;
       }

   for(i = 1;i < (xdim-1); i++)
     for(j = 1;j < (ydim-1); j++)
       for(k = 1;k < (zdim-1); k++)
	 {
          tempx = 0;
          tempy = 0;
          tempz = 0;
          for(l = 0; l < 3; l++)
            for(m = 0; m < 3; m++)
              for(n = 0; n < 3; n++)
               {
                index1 = (k+l-1)*xdim*ydim + (i+m-1)*ydim + (j+n-1);
                index2 = l*9 + m*3 + n;
                tempx = tempx + (float)(data[index1]*dx[index2])*0.037037;
                tempy = tempy + (float)(data[index1]*dy[index2])*0.037037;
                tempz = tempz + (float)(data[index1]*dz[index2])*0.037037;
               }
          *(xyangle + k*xdim*ydim + i*ydim + j) = acos(sqrt(tempx*tempx + tempz*tempz)/sqrt(tempx*tempx + tempy*tempy + tempz*tempz));

         }
}

surface_point * gradient(bframe3d * data, int x, int y, int z)
{
  surface_point * p = new surface_point;
  int l, m, n, index2;
  int value, tempx, tempy, tempz;

  p->x = x;
  p->y = y;
  p->z = z;

  tempx = 0;
  tempy = 0;
  tempz = 0;

  for(l = 0; l < 3; l++)
    for(m = 0; m < 3; m++)
      for(n = 0; n < 3; n++)
        {
          index2 = l*9 + m*3 + n;
          if(data->getpixel(x+l-1, y+m-1, z+n-1) == TRUE)
            value = 255;
          else
            value = 0;
          tempx = tempx + value*dx[index2];
          tempy = tempy + value*dy[index2];
          tempz = tempz + value*dz[index2];
        }
   p->gx = ((double)(tempx))*0.037037;
   p->gy = ((double)(tempy))*0.037037;
   p->gz = ((double)(tempz))*0.037037;

   return p;

}
