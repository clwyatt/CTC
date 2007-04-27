// File: read_rvf.cc
// Abstract: read rvf 16 bit image from file.
//
// Created: 07/21/98 by: Chris L. Wyatt
//
//                
#include "read_rvf.h"

#include "ctcConfigure.h"

using namespace std;

// byte swap function
template< class T >
inline void swab( T& t ) {
  assert( sizeof( T ) % 2 == 0 );
  char* ca = reinterpret_cast< char* >( &t );
  std::reverse( ca, ca + sizeof( T ) );
};

struct RVF_header
{
   int32_t magic;
   float version;

   // an identifying number for the rvf volume
   int32_t serial;

   // the volume dimensions
   int32_t
      xDim,
      yDim,
      zDim,
      bits;

   // whether or not to flip the image displays
   int32_t
      x_flipX, x_flipY,
      y_flipX, y_flipY,
      z_flipX, z_flipY;

   // the patient coordinate system
   float
      xPat_org, xPat_step,
      yPat_org, yPat_step,
      zPat_org, zPat_step;

   // some patient information
   char
      patDir[256],
      patName[64],
      hospNum[64],
      exam[16],
      series[16], 
      studyDate[16],
      sliceThickness[16];

   // if true, an ascii dump of a sample dicom header follows the volume data
   int32_t dicomHeaderFollows;

   // if (dicomHeaderFollows), the size of that header
   int32_t dicomHeaderSize;
};

unsigned short * read_rvf_file(char * name, int * xdim, int * ydim, int * zdim)
{
        ifstream  infile;
	unsigned short * threedp;

	infile.open( name );

	RVF_header header;
	infile.read((char *) &header, sizeof(header));

	//swap if needed
	bool swabreq;
	if(!CTC_BIG_ENDIAN)
	  swabreq = true;
	else
	  swabreq = false;
	
	//swap if needed
	if(swabreq)
	  {
	    swab(header.magic);
	    swab(header.version);
	    swab(header.serial);
	    swab(header.xDim);
	    swab(header.yDim);
	    swab(header.zDim);
	    swab(header.bits);
	    swab(header.x_flipX);
	    swab(header.x_flipY);
	    swab(header.y_flipX);
	    swab(header.y_flipY);
	    swab(header.z_flipX);
	    swab(header.z_flipY);
	    swab(header.xPat_org);
	    swab(header.yPat_org);
	    swab(header.zPat_org);
	    swab(header.xPat_step);
	    swab(header.yPat_step);
	    swab(header.zPat_step);
	    swab(header.dicomHeaderFollows);
	    swab(header.dicomHeaderSize);
	  }

	unsigned long size[3];
	size[0]  = header.xDim;
	size[1]  = header.yDim;
	size[2]  = header.zDim;
    
	*xdim = (int)size[0];
        *ydim = (int)size[1];
        *zdim = (int)size[2];
	
        fprintf(stderr, "Volume size: %ix%ix%i\n", *xdim, *ydim, *zdim);
	
	// read voxels into buffer argument
	int totalsize = (*xdim)*(*ydim)*(*zdim);
	threedp = new unsigned short[totalsize];
	unsigned short temp;
	for(int k=0; k < size[2]*size[1]*size[0]; k++){
	  infile.read((char *)(&temp), sizeof(unsigned short));
	  if(swabreq) swab(temp);
	  if(temp == 63536) temp = 0; //remove ROI marker
	  *(threedp + k) = temp;
	}

	// close the file
	infile.close();

        return threedp;

}
