// File: grad3d.h
// Created: 07/23/98 by: Chris L. Wyatt
//
//
// header for grad3d.cc
//
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include "bvolume.h"
#include "surface_defs.h"


static int dx[27]={-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1};
static int dy[27]={-1,-1,-1,0,0,0,1,1,1,-1,-1,-1,0,0,0,1,1,1,-1,-1,-1,0,0,0,1,1,1};
static int dz[27]={-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1};

void conv3d(unsigned short *data, 
            int xdim, int ydim, int zdim,
            float * grad_mag);

void xyang3d(unsigned short *data, 
            int xdim, int ydim, int zdim,
            float * xyangle);

surface_point * gradient(bframe3d * data, int x, int y, int z);
