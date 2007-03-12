// File: region_grow.cc
// Abstract: Grow 3D 6-connected region
//
// Created: 02/10/98 by: Chris L. Wyatt
// Modified: 07/13/98 for use in seed_locator.cc
//
//                
#include "region_grow.h"

float * grad_mag = NULL;
float * xyangle = NULL;

bframe3d * grow_region(int org_x, int org_y, int org_z,
                  int xdim, int ydim, int zdim, int tol,
                  unsigned short *data)
{
   bframe3d * region;
   int x, y, z;
   int i, j, k, pixelx, pixely, pixelz, found;
   double pp;
   //float seed;
   rbnode * rbtree_nil;
   rbnode * rbtree_root;
   rbnode * node;

   region = new bframe3d;
   region->setdim(xdim, ydim, zdim);
   region->clear();

   //get seed
   //seed= *(data+ org_z*xdim*ydim + org_x*ydim + org_y);

   //fprintf(stderr, "Seed intensity is %f\n", seed);

   //setup red/black tree
   rbtree_nil = new rbnode;
   rbtree_nil->color = BLK_COLOR;
   rbtree_root = rbtree_nil;
   node = new rbnode;

   x= org_x;
   y= org_y;
   z= org_z;
   

       region->setpixel(x,y,z,TRUE);
       RBInsert(rbtree_root, x, y, z, xdim, ydim, node, rbtree_nil);
    
       while(rbtree_root != rbtree_nil)
	{
	  for(i = -1; i <= 1; i++)
	    for(j = -1; j <= 1; j++)
              for(k = -1; k <= 1; k++)
	      {
	       pixelx = x + i;
	       pixely = y + j;
	       pixelz = z + k;
    
	       if((pixelx >= 0 && pixelx < xdim) &&
		  (pixely >= 0 && pixely < ydim) &&
		  (pixelz >= 0 && pixelz < zdim) &&
                  ( check6(i, j, k) == 1 ))
		{
		 if((*(data + pixelz*xdim*ydim + pixelx*ydim + 
		    pixely) < tol) &&
		    (region->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   //fprintf(stderr, "(%i,%i, %i) = %i\n", x, y, z, *(data + z*xdim*ydim + x*ydim + y));
	   region->setpixel(x, y, z, TRUE);
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
	   }


   return region;
} 

bframe3d * grow_region_grad(bframe3d * mask, int tol,
                            unsigned short *data, float * size)
{

  int xdim, ydim, zdim;
  bframe3d * region;
  //float * grad_mag;
  int i, j, k, l, m, n, pixelx, pixely, pixelz, found, x, y, z;
  double pp, mean;
  rbnode * rbtree_nil;
  rbnode * rbtree_root;
  rbnode * node;

  mask->querydim(&xdim, &ydim, &zdim);

  //compute gradient
  if( grad_mag == NULL){
     grad_mag = new float[xdim*ydim*zdim];
     conv3d(data, xdim, ydim, zdim, grad_mag);
  } 

   //FILE * of;
   //of = fopen("gradient_mag.raw", "w");
   //fwrite(grad_mag, xdim*ydim*zdim, sizeof(*grad_mag), of);
   //fclose(of);

  region = new bframe3d;
  region->setdim(xdim, ydim, zdim);
  region->clear();

   //setup red/black tree
   rbtree_nil = new rbnode;
   rbtree_nil->color = BLK_COLOR;
   rbtree_root = rbtree_nil;
   node = new rbnode;

  *size = 0.0;

  //put all mask points into region
  //and surface point neighbors onto 
  //stack if they meet criteria
  for(i = 0; i < xdim; i++)
    for(j = 0; j < ydim; j++)
      for(k = 0; k < zdim; k++)
	{
         region->setpixel(i, j, k, mask->getpixel(i, j, k));
         if(region->getpixel(i, j, k) == TRUE) *size = *size + 1;

         if( is_on_surface(mask, i, j, k) == 1 )
	   {
	     for(l = -1; l <= 1; l++)
	       for(m = -1; m <= 1; m++)
		 for(n = -1; n <= 1; n++)
		   {
		     pixelx = i + l;
		     pixely = j + m;
		     pixelz = k + n;
		     
		     if((pixelx >= 0 && pixelx < xdim) &&
			(pixely >= 0 && pixely < ydim) &&
			(pixelz >= 0 && pixelz < zdim) &&
			( check6(l, m, n) == 1 ))
		       {
			 if((fabs(*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely)) < tol) &&
			    (mask->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   }
	}

  //now grow the region
  
  mean = 0;

  //pop top
  if(rbtree_root != rbtree_nil)
    {
	   x = rbtree_root->x;
	   y = rbtree_root->y;
	   z = rbtree_root->z;
	   region->setpixel(x, y, z, TRUE);
           mean = *(grad_mag + z*xdim*ydim + x*ydim + y);
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
    }

  //continue until stack empty

       while(rbtree_root != rbtree_nil)
	{
	  for(i = -1; i <= 1; i++)
	    for(j = -1; j <= 1; j++)
              for(k = -1; k <= 1; k++)
	      {
	       pixelx = x + i;
	       pixely = y + j;
	       pixelz = z + k;
    
	       if((pixelx >= 1 && pixelx < xdim-1) &&
		  (pixely >= 1 && pixely < ydim-1) &&
		  (pixelz >= 1 && pixelz < zdim-1) &&
                  ( check6(i, j, k) == 1 ))
		{
//		 if((fabs(*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely)-mean) < tol) && //
		 if((fabs(*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely)) < tol) &&

		    (region->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   region->setpixel(x, y, z, TRUE);
           *size = *size + 1;
           mean = (mean + (float)(*(data + z*xdim*ydim + x*ydim + y)))/2.0;
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
	}

  fprintf(stderr, "Mean was %f\n", mean);

  //delete [] grad_mag;

  return region;
}


bframe3d * grow_region_grad2(bframe3d * mask, int tol,
                            unsigned short *data)
{

  int xdim, ydim, zdim;
  bframe3d * region;
  //float * grad_mag;
  int i, j, k, l, m, n, pixelx, pixely, pixelz, found, x, y, z;
  double pp, mean;
  rbnode * rbtree_nil;
  rbnode * rbtree_root;
  rbnode * node;

  mask->querydim(&xdim, &ydim, &zdim);

  //compute gradient
  if( grad_mag == NULL){
     grad_mag = new float[xdim*ydim*zdim];
     conv3d(data, xdim, ydim, zdim, grad_mag);
   FILE * of;
   of = fopen("gradient_mag.raw", "w");
   fwrite(grad_mag, xdim*ydim*zdim, sizeof(*grad_mag), of);
   fclose(of);
  } 



  region = new bframe3d;
  region->setdim(xdim, ydim, zdim);
  region->clear();

   //setup red/black tree
   rbtree_nil = new rbnode;
   rbtree_nil->color = BLK_COLOR;
   rbtree_root = rbtree_nil;
   node = new rbnode;


  //put all mask points into region
  //and surface point neighbors onto 
  //stack if they meet criteria
  for(i = 0; i < xdim; i++)
    for(j = 0; j < ydim; j++)
      for(k = 0; k < zdim; k++)
	{
         region->setpixel(i, j, k, mask->getpixel(i, j, k));
         if( is_on_surface(mask, i, j, k) == 1 )
	   {
	     for(l = 0; l <= 1; l++)
	       for(m = 0; m <= 0; m++)
		 for(n = 0; n <= 0; n++)
		   {
		     pixelx = i + l;
		     pixely = j + m;
		     pixelz = k + n;
		     
		     if((pixelx >= 0 && pixelx < xdim) &&
			(pixely >= 0 && pixely < ydim) &&
			(pixelz >= 0 && pixelz < zdim) &&
			( check6(l, m, n) == 1 ))
		       {
			 if((*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely) < *(grad_mag + k*xdim*ydim + i*ydim + j)) &&
			    (mask->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   }
	}

  //now grow the region
  
  mean = 0;

  //pop top
  if(rbtree_root != rbtree_nil)
    {
	   x = rbtree_root->x;
	   y = rbtree_root->y;
	   z = rbtree_root->z;
	   region->setpixel(x, y, z, TRUE);
           mean = *(grad_mag + z*xdim*ydim + x*ydim + y);
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
    }

  //continue until stack empty

       while(rbtree_root != rbtree_nil)
	{
	  for(i = 0; i <= 1; i++)
	    for(j = 0; j <= 0; j++)
              for(k = 0; k <= 0; k++)
	      {
	       pixelx = x + i;
	       pixely = y + j;
	       pixelz = z + k;
    
	       if((pixelx >= 1 && pixelx < xdim-1) &&
		  (pixely >= 1 && pixely < ydim-1) &&
		  (pixelz >= 1 && pixelz < zdim-1) &&
                  ( check6(i, j, k) == 1 ))
		{
		 if((*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely) < *(grad_mag + z*xdim*ydim + x*ydim + y)) &&

		    (region->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   region->setpixel(x, y, z, TRUE);
           mean = (mean + *(grad_mag + z*xdim*ydim + x*ydim + y))/2.0;
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
	}

  fprintf(stderr, "Mean was %f\n", mean);

  //delete [] grad_mag;

  return region;
}

bframe3d * grow_region_grad3(bframe3d * mask1, bframe3d * mask2, int tol,
                            unsigned short *data, float * size)
{

  int xdim, ydim, zdim;
  bframe3d * region;
  //float * grad_mag;
  int i, j, k, l, m, n, pixelx, pixely, pixelz, found, x, y, z;
  double pp, mean;
  rbnode * rbtree_nil;
  rbnode * rbtree_root;
  rbnode * node;

  int * blabel, lnum = 0, oneShot;

  mask1->querydim(&xdim, &ydim, &zdim);

  //compute gradient
  if( grad_mag == NULL){
     grad_mag = new float[xdim*ydim*zdim];
     conv3d(data, xdim, ydim, zdim, grad_mag);
  } 

   //FILE * of;
   //of = fopen("gradient_mag.raw", "w");
   //fwrite(grad_mag, xdim*ydim*zdim, sizeof(*grad_mag), of);
   //fclose(of);

  region = new bframe3d;
  region->setdim(xdim, ydim, zdim);
  region->clear();


  blabel = label(mask1, &lnum);

   //setup red/black tree
   rbtree_nil = new rbnode;
   rbtree_nil->color = BLK_COLOR;
   rbtree_root = rbtree_nil;
   node = new rbnode;

  *size = 0.0;

fprintf(stderr, "Using %i different local regions.\n", lnum);

for(int count = 1; count <= lnum; count ++)
{
  fprintf(stderr, "Segmenting Fluid in region %i. size is at %f.\n", count, *size);
  oneShot = 0;

  //put all points labelled count into region
  //and surface point neighbors onto 
  //stack if they meet criteria
  for(i = 0; i < xdim; i++)
    for(j = 0; j < ydim; j++)
      for(k = 0; k < zdim; k++)
	{
         if( (*(blabel + k*xdim*ydim + i*ydim + j) == count) && (mask2->getpixel(i, j, k) != TRUE) )
         {
           if(oneShot == 0){fprintf(stderr, "(%i,%i,%i)\n", i, j, k); oneShot = 1;}

           region->setpixel(i, j, k, TRUE);
           *size = *size + 1;

           if( is_on_surface(mask1, i, j, k) == 1 )
	   {
	     for(l = -1; l <= 1; l++)
	       for(m = -1; m <= 1; m++)
		 for(n = -1; n <= 1; n++)
		   {
		     pixelx = i + l;
		     pixely = j + m;
		     pixelz = k + n;
		     
		     if((pixelx >= 0 && pixelx < xdim) &&
			(pixely >= 0 && pixely < ydim) &&
			(pixelz >= 0 && pixelz < zdim) &&
			( check6(l, m, n) == 1 ))
		       {
			 if((fabs(*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely)) < tol) &&
			    (mask1->getpixel(pixelx, pixely, pixelz) != TRUE) &&
			    (mask2->getpixel(pixelx, pixely, pixelz) != TRUE))
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
            }
	   }
	}

  //now grow the region
  
  mean = 0;

  //pop top
  if(rbtree_root != rbtree_nil)
    {
	   x = rbtree_root->x;
	   y = rbtree_root->y;
	   z = rbtree_root->z;
	   region->setpixel(x, y, z, TRUE);
           mean = (float)(*(data + z*xdim*ydim + x*ydim + y));
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
    }

  //continue until stack empty

       while(rbtree_root != rbtree_nil)
	{
	  for(i = -1; i <= 1; i++)
	    for(j = -1; j <= 1; j++)
              for(k = -1; k <= 1; k++)
	      {
	       pixelx = x + i;
	       pixely = y + j;
	       pixelz = z + k;
    
	       if((pixelx >= 1 && pixelx < xdim-1) &&
		  (pixely >= 1 && pixely < ydim-1) &&
		  (pixelz >= 1 && pixelz < zdim-1) &&
                  ( check6(i, j, k) == 1 ))
		{
//		 if((fabs(*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely)-mean) < tol) && //
		 if((fabs(*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely)) < tol) &&
		    (region->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   region->setpixel(x, y, z, TRUE);
           *(blabel + z*xdim*ydim + x*ydim + y) = count;
           *size = *size + 1;
           mean = (mean + (float)(*(data + z*xdim*ydim + x*ydim + y)))/2.0;
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
	}

  fprintf(stderr, "Mean was %f\n", mean);

  // use mean from gradient-constrained region to segment using intensity

  //put all surface point neighbors onto 
  //stack if they meet criteria
  for(i = 0; i < xdim; i++)
    for(j = 0; j < ydim; j++)
      for(k = 0; k < zdim; k++)
	{
         if( *(blabel + k*xdim*ydim + i*ydim + j) == count )
         {
           if( is_on_surface(region, i, j, k) == 1 )
      	   {
	       for(l = -1; l <= 1; l++)
	         for(m = -1; m <= 1; m++)
		   for(n = -1; n <= 1; n++)
		     {
		       pixelx = i + l;
		       pixely = j + m;
		       pixelz = k + n;
		     
		       if((pixelx >= 0 && pixelx < xdim) &&
			  (pixely >= 0 && pixely < ydim) &&
			  (pixelz >= 0 && pixelz < zdim) &&
			  ( check6(l, m, n) == 1 ))
		         {
			   if((fabs((float)(*(data + pixelz*xdim*ydim + pixelx*ydim + pixely)) - mean) < 100) &&
			      (region->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   }
         }
	}

  //now grow the intensity region

  //pop top
  if(rbtree_root != rbtree_nil)
    {
	   x = rbtree_root->x;
	   y = rbtree_root->y;
	   z = rbtree_root->z;
	   region->setpixel(x, y, z, TRUE);
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
    }

  //continue until stack empty

       while(rbtree_root != rbtree_nil)
	{
	  for(i = -1; i <= 1; i++)
	    for(j = -1; j <= 1; j++)
              for(k = -1; k <= 1; k++)
	      {
	       pixelx = x + i;
	       pixely = y + j;
	       pixelz = z + k;
    
	       if((pixelx >= 1 && pixelx < xdim-1) &&
		  (pixely >= 1 && pixely < ydim-1) &&
		  (pixelz >= 1 && pixelz < zdim-1) &&
                  ( check6(i, j, k) == 1 ))
		{
		 if((fabs((float)(*(data + pixelz*xdim*ydim + pixelx*ydim + pixely)) - mean) < 100) &&
		    (region->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   region->setpixel(x, y, z, TRUE);
           *size = *size + 1;
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
	}

  //delete [] grad_mag;

}

  return region;
}

bframe3d * grow_region_pv(bframe3d * mask, float tol,
                            unsigned short *data)
{

  int xdim, ydim, zdim;
  bframe3d * region;
  //float * xyangle;
  int i, j, k, l, pixelx, pixely, pixelz, found, x, y, z;
  double pp, mean;
  rbnode * rbtree_nil;
  rbnode * rbtree_root;
  rbnode * node;

  mask->querydim(&xdim, &ydim, &zdim);

  //compute xyangle
  if( xyangle == NULL ){
     xyangle = new float[xdim*ydim*zdim];
     xyang3d(data, xdim, ydim, zdim, xyangle);

     //FILE * of;
     //of = fopen("xyangle.raw", "w");
     //fwrite(xyangle, xdim*ydim*zdim, sizeof(*xyangle), of);
     //fclose(of);
  }

  region = new bframe3d;
  region->setdim(xdim, ydim, zdim);
  region->clear();

   //setup red/black tree
   rbtree_nil = new rbnode;
   rbtree_nil->color = BLK_COLOR;
   rbtree_root = rbtree_nil;
   node = new rbnode;


  //put all mask points into region
  //and surface point neighbors onto 
  //stack if they meet criteria
  for(i = 0; i < xdim; i++)
    for(j = 0; j < ydim; j++)
      for(k = 0; k < zdim; k++)
	{
         region->setpixel(i, j, k, mask->getpixel(i, j, k));
         if( is_on_surface(mask, i, j, k) == 1 )
	   {
	     for(l = -1; l <= 1; l++)
	       //for(m = -1; m <= 1; m++)
	       //for(n = -1; n <= 1; n++)
		   {
		     pixelx = i + l;
                     pixely = j; pixelz = k;
		     //pixely = j + m;
		     //pixelz = k + n;
		     
		     if((pixelx >= 0 && pixelx < xdim) &&
			(pixely >= 0 && pixely < ydim) &&
			(pixelz >= 0 && pixelz < zdim) )//&&
			//( check6(l, m, n) == 1 ))
		       {
			 if((fabs(*(xyangle + pixelz*xdim*ydim + pixelx*ydim + pixely)-1.570796327) < tol) &&
			    (mask->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   }
	}

  //now grow the region
  
  mean = 0;

  //pop top
  if(rbtree_root != rbtree_nil)
    {
	   x = rbtree_root->x;
	   y = rbtree_root->y;
	   z = rbtree_root->z;
	   region->setpixel(x, y, z, TRUE);
           mean = *(xyangle + z*xdim*ydim + x*ydim + y);
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
    }

  //continue until stack empty

       while(rbtree_root != rbtree_nil)
	{
	  for(i = -1; i <= 1; i++)
	    //for(j = -1; j <= 1; j++)
	    //for(k = -1; k <= 1; k++)
	      {
	       pixelx = x + i;
               pixely = y; pixelz = z;
	       //pixely = y + j;
	       //pixelz = z + k;
    
	       if((pixelx >= 1 && pixelx < xdim-1) &&
		  (pixely >= 1 && pixely < ydim-1) &&
		  (pixelz >= 1 && pixelz < zdim-1) )//&&
                  //( check6(i, j, k) == 1 ))
		{
//		 if((fabs(*(grad_mag + pixelz*xdim*ydim + pixelx*ydim + pixely)-mean) < tol) && //
		 if((fabs(*(xyangle + pixelz*xdim*ydim + pixelx*ydim + pixely)-1.570796327) < tol) &&

		    (region->getpixel(pixelx, pixely, pixelz) != TRUE))
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
	   region->setpixel(x, y, z, TRUE);
           mean = (mean + *(xyangle + z*xdim*ydim + x*ydim + y))/2.0;
	   RBDelete(rbtree_root, rbtree_root->key, rbtree_nil);
	}

  fprintf(stderr, "Mean was %f\n", mean);

  //delete [] xyangle;

  return region;
}


int check6(int i, int j, int k)
{
  int flg;

  flg = 0;

  if( (i == 0) && (j == 0) && (k == 1) )
    flg = 1;
  if( (i == 0) && (j == 0) && (k == -1) )
    flg = 1;
  if( (i == 0) && (j == 1) && (k == 0) )
    flg = 1;
  if( (i == 0) && (j == -1) && (k == 0) )
    flg = 1;
  if( (i == 1) && (j == 0) && (k == 0) )
    flg = 1;
  if( (i == -1) && (j == 0) && (k == 0) )
    flg = 1;
  
  return flg;
}
