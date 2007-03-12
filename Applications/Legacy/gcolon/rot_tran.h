// File: rot_tran.h
// Created: 07/27/98 by: Chris L. Wyatt
//
//
// header for rot_tran.cc
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "surface_defs.h"

point Rx(point p, double angle);

point Ry(point p, double angle);

point Rz(point p, double angle);

point Txyz(point p, double dx, double dy, double dz);
