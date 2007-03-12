// File: elongation.h
// Abstract: header for elongation.cc
// Created: 01/14/99 by: Chris L. Wyatt
// Modified:
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bvolume.h"

float elongation1(bframe3d * input);

int is_on_surface(bframe3d * data, int x, int y, int z);
