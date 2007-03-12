// File: region_grow.h
// Abstract: header for region_grow.cc
// Created: 02/10/98 by: Chris L. Wyatt
// Modified: 07/13/98 for use in seed_locator.cc
//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bvolume.h"
#include "rbtree.h"
#include "canny.h"
#include "morph.h"


bframe3d * grow_region(int org_x, int org_y, int org_z,
                  int xdim, int ydim, int zdim, int tol,
                  unsigned short *data);

bframe3d * grow_region_grad(bframe3d * mask, int tol,
                            unsigned short *data, float * size);

bframe3d * grow_region_grad2(bframe3d * mask, int tol,
                            unsigned short *data);

bframe3d * grow_region_grad3(bframe3d * mask1, bframe3d * mask2, int tol,
                            unsigned short *data, float * size);

bframe3d * grow_region_pv(bframe3d * mask, float tol,
                            unsigned short *data);

int check6(int i, int j, int k);

int is_on_surface(bframe3d * data, int x, int y, int z);

void conv3d(unsigned short *data, 
            int xdim, int ydim, int zdim,
            float * grad_mag);

void xyang3d(unsigned short *data, 
            int xdim, int ydim, int zdim,
            float * xyangle);
