// File: plane.h
// Abstract: header for plane.cc
// Created: 08/21/98 by: Chris L. Wyatt
// Modified:
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bvolume.h"
#include "rbtree.h"
#include "surface_defs.h"
#include "morph.h"
//#include "grad3d.h"
#include "curvature.h"
#include "region_grow.h"

void select_on_plane(bframe3d * colon, 
                     bframe3d * colon_flat,
                     surface_point * surf, int numsp,
                     float tol);

float L(float A, float B, float C, float D, int x, int y, int z);


