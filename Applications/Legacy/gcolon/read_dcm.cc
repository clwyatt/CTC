// File: read_dcm.cc
// Abstract: read raw 16 bit image from (not part 10) DICOM file.
//
// Created: 07/13/98 by: Chris L. Wyatt
//
//                
#include "read_dcm.h"

using namespace std;

unsigned short * read_dcm_file(char * name,
                              int xdim,
                              int ydim,
                              int zdim,
                              int offset)
{
        ifstream  infile;
        ifstream  header;
        char filename[100];
        char *temp2, b1, b2;
	unsigned short * threedp;
        int i;
	unsigned short group, element, temp[10];
      
        threedp = new unsigned short[xdim*ydim*zdim];

        header.open( name );

        //dump top 3 lines
        header>>filename;
        header>>filename;
        header>>filename;
        header>>filename;

        for(i=0; i < zdim; i++)
	  {
            header>>filename;

            fprintf(stderr, "Reading DICOM file %s.\n", filename);

	    infile.open( filename );
            group =  0; element = 0;
            // scan for PXL data
            while ( ((group != 0xe07f) || (element != 0x1000)) && !infile.eof() )
              {
                infile.read((char *)(&group), 2);
                if ( (group == 0xe07f) )
                   infile.read((char *)(&element), 2);
                //fprintf(stderr, "G = %#x, E = %#x \n", group, element);
              }

            if ( (group == 0xe07f) && (element == 0x1000) ){
                fprintf(stderr, "PXL data found.\n");
                infile.read((char *)(&temp), 8);
             }

            if ( infile.eof() ) 
                fprintf(stderr, "EOF reached.\n");

            infile.read((char *)(threedp+ i*xdim*ydim), 2*xdim*ydim);

	    infile.close();

          }

       header.close();

       // swap byte order and set 32768 to 0
       for(i = 0; i < xdim*ydim*zdim; i++)
        {
          temp2 = (char *)(threedp + i);
          b1 = *temp2;
	  b2 = *(temp2+1);
          *temp2 = b2;
          *(temp2+1) = b1;

	  if( *(threedp + i) == 32768 ) *(threedp + i) = 0;
        }
       return threedp;

}
