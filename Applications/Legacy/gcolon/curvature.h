// File: curvature.h
// Abstract: header for curvature.cc
// Created: 07/24/98 by: Chris L. Wyatt
// Modified: 07/28/98 for use in seed_locator.cc
//
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bvolume.h"
#include "grad3d.h"
#include "rot_tran.h"
#include "free_ivector.h"
#include "free_matrix.h"
#include "invertMatrix.h"
#include "matrix.h"
#include "svbksb.h"
#include "svdcmp.h"
#include "vector.h"


surface_point * curvature(bframe3d * data, int * surface_length);

int is_on_surface(bframe3d * data, int x, int y, int z);

double gxangle(surface_point * p);
double gyangle(surface_point * p);
double gzangle(surface_point * p);

/* void write_curvature(surface_point * surf, int surface_length, int volsize); */
