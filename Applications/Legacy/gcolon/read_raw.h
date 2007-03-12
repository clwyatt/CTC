// File: read_raw.h
// Created: 07/13/98 by: Chris L. Wyatt
//
//
// header for read_raw.cc
//
//

#include <stdlib.h>
#include <stdio.h>
#include <fstream>


unsigned short * read_img_file(char * name,
                              int xdim,
                              int ydim,
                              int zdim,
                              int offset);
