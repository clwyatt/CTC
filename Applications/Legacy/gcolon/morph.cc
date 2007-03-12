// File: morph.cc
// Abstract: morphological type operators
//
// Created: 07/31/98 by: Chris L. Wyatt
// Modified:
//
//                
#include "morph.h"

void select_area(bframe3d * input, int tol)
{
   int i, j, k, l, a;
   int xdim, ydim, zdim, num;
   int * area_num, * blabel;

   input->querydim(&xdim, &ydim, &zdim);

   area_num = area(input, &num, blabel); 

   for(i = 0; i < xdim; i++)
     for(j = 0; j < ydim; j++)
       for(k = 0; k < zdim; k++)
        {
         l = *(blabel + k*xdim*ydim + i*ydim + j);
         if(l > 0)
	   {
            a = *(area_num + l-1);
            if(a < tol)
               input->setpixel(i, j, k, FALSE);
           }
	}

   delete [] area_num;
   delete [] blabel;
}

int * area(bframe3d * input, int * lnum, int * &blabel)
{
   int i, j, k, l;
   int xdim, ydim, zdim;
   int * output;

   input->querydim(&xdim, &ydim, &zdim);

   blabel = label(input, lnum);

   output = new int[*lnum];

   for(i = 0; i < *lnum; i++)
     *(output + i) = 0;

   for(i = 0; i < xdim; i++)
     for(j = 0; j < ydim; j++)
       for(k = 0; k < zdim; k++)
	 {
          l = *(blabel + k*xdim*ydim + i*ydim + j);
          if(l > 0)
            *(output + (l-1)) += 1;
         }

//   for(i = 0; i < *lnum; i++)
//     fprintf(stderr, "Area %i = %i\n", i, *(output + i));

  return output;
}


int * label(bframe3d * input, int * num)
{

   int xdim, ydim, zdim;
   int i, j, k, l, m, n, count, index;
   int x, y, z, pixelx, pixely, pixelz, found;
   double pp;
   rbnode * rbtree_nil;
   rbnode * rbtree_root;
   rbnode * node;
   int * output;

   input->querydim(&xdim, &ydim, &zdim);
   output = new int[xdim*ydim*zdim];

   //clear all to zero
   for(i = 1; i < xdim*ydim*zdim; i++)
      *(output + i) = 0;

   count = 1;

   for(i = 0; i < xdim; i++)
     for(j = 0; j < ydim; j++)
       for(k = 0; k < zdim; k++)
	 {
           index = k*xdim*ydim + i*ydim + j;

           if( (input->getpixel(i, j, k) == TRUE) && (*(output + index) == 0) )
	     {
               //setup red/black tree
               rbtree_nil = new rbnode;
               rbtree_nil->color = BLK_COLOR;
               rbtree_root = rbtree_nil;
               node = new rbnode;

               *(output + index) = count;
               RBInsert(rbtree_root, i, j, k, xdim, ydim, node, rbtree_nil);

               x = i; y = j; z = k;
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
			       if( (input->getpixel(pixelx, pixely, pixelz) == TRUE) &&
                                   (*(output + pixelz*xdim*ydim + pixelx*ydim + pixely) == 0) )
				 {
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
		   *(output + z*xdim*ydim + x*ydim + y) = count;
		   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
		 }
              count += 1;
	     }
	 }

   fprintf(stderr, "%i labels used.\n", (count-1));

   *num = count - 1;
   return output;
}


bframe3d * merge(bframe3d * in1, bframe3d * in2)
{
  int xdim, ydim, zdim;
  int i, j, k;
  bframe3d * out;

  in1->querydim(&xdim, &ydim, &zdim);

  out = new bframe3d;
  out->setdim(xdim, ydim, zdim);
  out->clear();

  for(i = 0; i < xdim; i++)
    for(j = 0; j < ydim; j++)
      for(k = 0; k < zdim; k++)
       {
        if( (in1->getpixel(i, j, k) == TRUE) || (in2->getpixel(i, j, k) == TRUE) )
          out->setpixel(i, j, k, TRUE);
       }

  return out;
}

//bframe3d * thin(bframe3d * in)
//{
//  int xdim, ydim, zdim;
//  int i, j, k;
//  bframe3d * out;
//
//  in->querydim(&xdim, &ydim, &zdim);
//
//  out = new bframe3d;
//  out->setdim(xdim, ydim, zdim);
//  out->clear();
//
//  for(i = 0; i < xdim; i++)
//    for(j = 0; j < ydim; j++)
//      for(k = 0; k < zdim; k++)
//       {
//        if( in->getpixel(i, j, k) == TRUE )
//          out->setpixel(i, j, k, TRUE);
//       }
//
//  return out;
//
//}
