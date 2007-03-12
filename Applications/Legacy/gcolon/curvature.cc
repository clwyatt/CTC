// File: curvature.cc
// Abstract: determine curvature of surface points on binary volume
//
// ref. Tracing Surfaces for Surfacing Traces
//      Sander & Zucker
//
// ref. Surface Parameterization and Curvature Measurement
//      of Arbitrary 3-D Objects: Five Pratical Methods
//      Stokely and Wu, PAMI vol. 14, 1992
//
// Created: 02/24/98 by: Chris L. Wyatt
// Modified: 07/28/98 for use in seed_locator.cc
//
//                
#include "curvature.h"

surface_point * curvature(bframe3d * data, int * surface_length)
{
  surface_point * surf_top;
  surface_point * current;
  surface_point * temp;
  int xdim, ydim, zdim;
  int i, j, k, x1, x2, y1, y2, z1, z2;
  double Ax, Ay, Az, dx, dy, dz;
  double sum11, sum12, sum13, sum22, sum23, sum33;
  double a, b, c;
  point ip, p; // p1, p2, p3;
  point neighbors[100];
  int numn, sp;
  float **ATA, **ATAInv;

  *surface_length = 0;
  data->querydim(&xdim, &ydim, &zdim);
  surf_top = NULL;
  current = surf_top;

  //loop through adding to list of points
  //cheat a little and don't check edges
  for(i = 1; i < (xdim-1); i++)
    for(j = 1; j < (ydim-1); j++)
      for(k = 1; k < (zdim-1); k++)
        {
         if( is_on_surface(data, i, j, k) == 1 )
          {
           temp = gradient(data, i, j, k);
           if(surf_top == NULL){
             surf_top = temp;
             current = surf_top;}
           else{
             current->next = temp;
             current = temp;
            }
           *surface_length += 1;
          }
        }

  fprintf(stderr, "%i surface points found.\n", *surface_length);

  current = surf_top;
  for(sp = 0; sp < *surface_length; sp++){
 
    x2 = current->x;
    y2 = current->y;
    z2 = current->z;

    //fprintf(stderr, "Center point: (%i,%i,%i)\n", x2, y2, z2);


    Ax = gxangle(current);
    Ay = gyangle(current);
    Az = gzangle(current);

    //fprintf(stderr, "Rotation:\nRx=%f \nRy=%f \nRz=%f\n\n", Ax, Ay, Az);

    numn = 0;

    for(i=-2; i < 3; i++)
      for(j=-2; j < 3; j++)
	for(k=-2; k < 3; k++)
	  {
	    x1 = x2+i; y1 = y2+j; z1 = z2+k;
	    if(is_on_surface(data, x1, y1, z1) == TRUE)
	      {
		ip.x =  (double)(x2) - (double)(x1);
		ip.y =  (double)(y2) - (double)(y1);
		ip.z =  (double)(z2) - (double)(z1);
                //p1 = Rz(ip, Az);
                //p2 = Ry(p1, Ay);
                //p3 = Rx(p2, Ax);
		p = Rx(Ry(Rz(ip, Az), Ay), Ax); 
		neighbors[numn] = p;
		numn +=1;
		//fprintf(stderr, "Origional point %i: (%i,%i,%i)\n", numn, x1, y1, z1);
		//fprintf(stderr, "Rz: (%f,%f,%f)\n", p1.x, p1.y, p1.z);
		//fprintf(stderr, "Ry: (%f,%f,%f)\n", p2.x, p2.y, p2.z);
		//fprintf(stderr, "Rx: (%f,%f,%f)\n", p3.x, p3.y, p3.z);
		//fprintf(stderr, "New Point %i: (%f, %f, %f)\n", numn, p.x, p.y, p.z);
	      }
	  }

    numn -= 1;
  
    ATA = matrix(1, 3, 1, 3);
    ATAInv = matrix(1, 3, 1, 3);

    sum11 = 0; sum12 = 0; sum13 = 0;
    sum22 = 0; sum23 = 0; sum33 = 0;
    for (i=0; i<numn; i++){
      dx = neighbors[i].x;	
      dy = neighbors[i].y;	
      sum11 += dx*dx*dx*dx;
      sum12 += dx*dx*2*dx*dy;
      sum13 += dx*dx*dy*dy;
      sum22 += 2*dx*dy*2*dx*dy;
      sum23 += 2*dx*dy*dy*dy;
      sum33 += dy*dy*dy*dy;
    }

    ATA[1][1] = sum11;
    ATA[1][2] = sum12;
    ATA[1][3] = sum13;
    ATA[2][1] = sum12;
    ATA[2][2] = sum22;
    ATA[2][3] = sum23;
    ATA[3][1] = sum13;
    ATA[3][2] = sum23;
    ATA[3][3] = sum33;

    invertMatrixSVD(ATA, ATAInv, 3);

    sum11 = sum12 = sum13 = 0;
    for (i=0; i<numn; i++){
      dx = neighbors[i].x;	
      dy = neighbors[i].y;	
      dz = neighbors[i].z;	
      sum11 += dx*dx*dz;	
      sum12 += 2*dx*dy*dz;	
      sum13 += dy*dy*dz;	
    }

    a = ATAInv[1][1]*sum11 + ATAInv[1][2]*sum12 + ATAInv[1][3]*sum13; 
    b = ATAInv[2][1]*sum11 + ATAInv[2][2]*sum12 + ATAInv[2][3]*sum13; 
    c = ATAInv[3][1]*sum11 + ATAInv[3][2]*sum12 + ATAInv[3][3]*sum13; 

    current->K = 4*(a*c - b*b);
    current->H = a + c;

    //fprintf(stderr, "K = %f, ", current->K);
    //fprintf(stderr, "H = %f\n", current->H);

    free_matrix(ATA, 1, 3, 1, 3);
    free_matrix(ATAInv, 1, 3, 1, 3);

    current = current->next;
  }


  return surf_top;

}


int is_on_surface(bframe3d * data, int x, int y, int z)
{

  int flg = 0;

  if(data->getpixel(x, y, z) == TRUE){
    if( (data->getpixel(x+1, y, z) != TRUE) ||
        (data->getpixel(x-1, y, z) != TRUE) ||
        (data->getpixel(x, y+1, z) != TRUE) ||
        (data->getpixel(x, y-1, z) != TRUE) ||
        (data->getpixel(x, y, z+1) != TRUE) ||
        (data->getpixel(x, y, z-1) != TRUE) )
      {
        flg = 1;
      }
   }

  return flg;
}


double gxangle(surface_point * p)
{
  double Ax;

  Ax = atan2(p->gz, p->gy);

  return Ax;
}

double gyangle(surface_point * p)
{
  double Ay;

  Ay = atan2(p->gx, p->gz);

  return Ay;
}

double gzangle(surface_point * p)
{
  double Az;

  Az = atan2(p->gy, p->gx);

  return Az;
}

// void write_curvature(surface_point * surf, int surface_length, int volsize)
// {
//  surface_point * current;
//  float *H, *K;
//  int i;

//  H = new float[volsize];
//  k = new float[volsize];

//  for(i=0; i < volsize; i++)
//    {
//     *(H + i) = 0;
//     *(K + i) = 0;
//    }

//  current = surf;
//  for(i=0; i < surface_length; i++)
//    {
    
//    }
// }
