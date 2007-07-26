/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcCTCImage_h
#define __ctcCTCImage_h

#include "itkOrientedImage.h"

namespace ctc
{
  // all CTC images are of short type
  typedef short CTCPixelType;

  // all CTC images are 3D
  const int CTCDimension = 3;

  // The common CTC Image Type
  typedef itk::OrientedImage<CTCPixelType, CTCDimension> CTCImageType;

  // the common binary type
  typedef unsigned char                             BinaryPixelType;
  typedef itk::Image<BinaryPixelType, 3>            BinaryImageType;

  // the common projection image type
  typedef unsigned char                             ProjectionPixelType;
  typedef itk::Image<ProjectionPixelType, 2>        ProjectionImageType;

} // end namespace ctc

#endif // __ctcCTCImage_h
