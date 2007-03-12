// File: morph.h
// Abstract: header for morph.cc
// Created: 07/31/98 by: Chris L. Wyatt
// Modified:
//
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bvolume.h"
#include "rbtree.h"

void select_area(bframe3d * input, int tol);

int * area(bframe3d * input, int * lnum, int * &blabel);

int * label(bframe3d * input, int * num);

bframe3d * merge(bframe3d * in1, bframe3d * in2);

int check6(int i, int j, int k);
