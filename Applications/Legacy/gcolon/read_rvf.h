// File: read_rvf.h
// Created: 07/21/98 by: Chris L. Wyatt
//
//
// header for read_rvf.cc
//
//

#include <stdlib.h>
#include <stdio.h>
#include <fstream>


unsigned short * read_rvf_file(char * name, int * xdim, int * ydim, int * zdim);
