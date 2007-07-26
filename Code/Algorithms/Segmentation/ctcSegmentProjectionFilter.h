/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcSegmentProjectionFilter_h
#define __ctcSegmentProjectionFilter_h

#include "itkSmartPointer.h"
#include "itkImageToImageFilter.h"

#include "ctcCTCImage.h"

namespace ctc
{
  
class ITK_EXPORT SegmentProjectionFilter :
    public itk::ImageToImageFilter<BinaryImageType, ProjectionImageType>
{
 public:
 
  typedef SegmentProjectionFilter                        Self;
  typedef itk::ImageToImageFilter<BinaryImageType,
                                  ProjectionImageType>  Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;

  // standard ITK macros
  itkNewMacro(Self);
  itkTypeMacro(SegmentProjectionFilter, itk::ImageToImageFilter);

  void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  
protected:

  SegmentProjectionFilter();

  void GenerateOutputInformation();

  void GenerateData();

private:

  SegmentProjectionFilter(Self&);            // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented

};

} // end namespace ctc

#endif // __ctcSegmentProjectionFilter_h
