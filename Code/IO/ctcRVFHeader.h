/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id: ctcRVFHeader.h 23 2006-06-29 14:45:24Z clwyatt $
Language:  C++

The following was adapted from the original WFU region software
*******************************************************************************/

#ifndef __ctcRVFHeader_h
#define __ctcRVFHeader_h

namespace ctc
{
/*
An RVF (Region Volume Format) volume is a simple volume file that
contains a header followed by the binary volume data.  At the end of the
file, an ascii dump of a sample DICOM header can be stored.
The volume data is stored in the usual multi-dimensional array order:
   first row of first slice
   second row of first slice
   ...
   last row of first slice
   first row of second slice
   ...
The header structure will probably change in the future as more or less
information needs to be stored in the volume header.  The 'version' member
contains the version number of the header.
*/

#define RVF_MAGIC_NUMBER 1685485170
// magic == ('d'<<24 | 'v'<<16 | 'v'<<8 | 'r')

#include <inttypes.h>

struct RVF_header_1_6
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

typedef RVF_header_1_6 RVF_header;
#define RVF_CURRENT_VERSION 1.6

} // end namespace ctc

#endif // __ctcRVFHeader_h
