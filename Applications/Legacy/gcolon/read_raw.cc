// File: read_raw.cc
// Abstract: read raw 16 bit image given dimensions
//
// Created: 07/13/98 by: Chris L. Wyatt
//
//                
#include "read_raw.h"

using namespace std;

unsigned short * read_img_file(char * name,
                              int xdim,
                              int ydim,
                              int zdim,
                              int offset)
{
        ifstream  infile;
	unsigned short * threedp;
        int i;
      
        threedp = new unsigned short[xdim*ydim*zdim];

	infile.open( name );

        infile.seekg(offset);

	infile.read((char *)threedp, 2*xdim*ydim*zdim);

	infile.close();

        for(i=0; i < xdim*ydim*zdim; i++)
          if(*(threedp + i) == 32768) *(threedp+i) = 0;

       return threedp;

}
