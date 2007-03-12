// File: read_dcm.h
// Created: 07/14/98 by: Chris L. Wyatt
//
//
// header for read_dcm.cc
//
//

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string.h>

unsigned short * read_dcm_file(char * name,
                              int xdim,
                              int ydim,
                              int zdim,
                              int offset);
