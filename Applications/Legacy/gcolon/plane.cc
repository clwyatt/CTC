// File: plane.cc
// Abstract: identify local points within tolerance of planar patches
//
// Created: 08/21/98 by: Chris L. Wyatt
// Last Modified:
//                  
//

#include "plane.h"

void select_on_plane(bframe3d * colon, 
                     bframe3d * colon_flat,
                     surface_point * surf, int numsp,
                     float tol)
{
   int i, l, m, n;
   int x, y, z, found;
   int xdim, ydim, zdim;
   int lnum, * blabel, count;
   surface_point * current, * temp;
   surface_point * minHK;
   float * A, * B, * C, * D, nx, ny, nz; // tempL;
   int pixelx, pixely, pixelz;
   double pp;
   rbnode * rbtree_nil;
   rbnode * rbtree_root;
   rbnode * node;

   colon->querydim(&xdim, &ydim, &zdim);

   blabel = label(colon_flat, &lnum);
 
   minHK = new surface_point[lnum];

   //init minHK
   for(i=0; i < lnum; i++)
    {
     minHK[i].x = 0;
     minHK[i].y = 0;
     minHK[i].z = 0;
     minHK[i].H = 1000;
     minHK[i].K = 1000;
    }

   // find min H-K point in each labeled set
   current = surf;
   for(i = 0; i < numsp; i++)
    {
      x = current->x;
      y = current->y;
      z = current->z;

      l = *(blabel + z*xdim*ydim + x*ydim + y);

      if( l > 0 )
	{
          if( (fabs(current->H) < fabs(minHK[l-1].H)) && (fabs(current->K) < fabs(minHK[l-1].K)) )
           {
             minHK[l-1].x = current->x;
             minHK[l-1].y = current->y;
             minHK[l-1].z = current->z;
             minHK[l-1].H = current->H;
             minHK[l-1].K = current->K;
           }
        }
      current = current->next;
    }

   A = new float[lnum];
   B = new float[lnum];
   C = new float[lnum];
   D = new float[lnum];

   // get plane parameters for each labeled set
   for(l=0; l < lnum; l++)
    {
     temp = gradient(colon, minHK[l].x, minHK[l].y, minHK[l].z);
     nx = temp->gx; ny = temp->gy; nz = temp->gz;

     A[l] = (nx)/(sqrt(nx*nx + ny*ny + nz*nz));
     B[l] = (ny)/(sqrt(nx*nx + ny*ny + nz*nz));
     C[l] = (nz)/(sqrt(nx*nx + ny*ny + nz*nz));
     D[l] = A[l]*minHK[l].x + B[l]*minHK[l].y + C[l]*minHK[l].z;

     //printf("Px=%i, Py=%i, Pz=%i\n", minHK[l].x, minHK[l].y, minHK[l].z);
     //printf("nx=%f, ny=%f, nz=%f\n", nx, ny, nz);
     //printf("A=%f, B=%f, C=%f, D=%f\n\n", A[l], B[l], C[l], D[l]);
    }

   //clear colon_flat
   colon_flat->clear();

   // grow surface for each labeled set
   for(i=0; i < lnum; i++)
    {
      //setup red/black tree
      rbtree_nil = new rbnode;
      rbtree_nil->color = BLK_COLOR;
      rbtree_root = rbtree_nil;
      node = new rbnode;

      count = 0;

      RBInsert(rbtree_root, minHK[i].x, minHK[i].y, minHK[i].z, xdim, ydim, node, rbtree_nil);

      x = minHK[i].x; y = minHK[i].y; z = minHK[i].z;

      while(rbtree_root != rbtree_nil)
		 {
		   for(l = -1; l <= 1; l++)
		     for(m = -1; m <= 1; m++)
		       for(n = -1; n <= 1; n++)
			 {
			   pixelx = x + l;
			   pixely = y + m;
			   pixelz = z + n;
    
			   if((pixelx >= 0 && pixelx < xdim) &&
			      (pixely >= 0 && pixely < ydim) &&
			      (pixelz >= 0 && pixelz < zdim) &&
			      ( check6(l, m, n) == 1 ))
			     {
                               //tempL = L(A[i], B[i], C[i], D[i], pixelx, pixely, pixelz);
			       if( (colon_flat->getpixel(pixelx, pixely, pixelz) == FALSE) &&
                                   (is_on_surface(colon, pixelx, pixely, pixelz)) &&
                                   ( L(A[i], B[i], C[i], D[i], pixelx, pixely, pixelz) < tol) )
				 {
                                   //printf("Pushing %i,%i,%i with L = %f\n", pixelx, pixely, pixelz, tempL);
				   pp = pack(pixelx, pixely, pixelz, xdim, ydim);
				   BinTreeSearch(rbtree_root, pp, &found, rbtree_nil);
				   if(found == 0)
				     {
				       RBInsert(rbtree_root, pixelx, pixely, pixelz, xdim, ydim,
						node, rbtree_nil);
				     }
				 }
			     }
			 }

		   x = rbtree_root->x;
		   y = rbtree_root->y;
		   z = rbtree_root->z;
		   colon_flat->setpixel(x, y, z, TRUE);
		   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
                   count += 1;
                   //printf("Popping %i,%i,%i (#%i of %i)\n", x, y, z, count, i);
		 }
    }

   delete [] blabel;
}

//distance from point (x,y,z) to plane defined by A, B, C, D
float L(float A, float B, float C, float D, int x, int y, int z)
{
  float output;
  float den;

  den = sqrt(A*A + B*B + C*C);

  output = (fabs(A*x + B*y + C*z - D))/den;

  return output;
}
