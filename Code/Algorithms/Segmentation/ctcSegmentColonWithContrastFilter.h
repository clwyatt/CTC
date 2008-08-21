/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcSegmentColonWithContrastFilter_h
#define __ctcSegmentColonWithContrastFilter_h

#include "itkSmartPointer.h"
#include "itkImageToImageFilter.h"
#include "itkChangeLabelImageFilter.h"
#include "itkScalarImageKmeansImageFilter.h"
#include "itkSliceBySliceImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkMinMaxCurvatureFlowImageFilter.h"

#include "ctcCTCImage.h"

namespace ctc
{
  
class ITK_EXPORT SegmentColonWithContrastFilter :
    public itk::ImageToImageFilter<CTCImageType, BinaryImageType>
{
 public:
 
  typedef SegmentColonWithContrastFilter                        Self;
  typedef itk::ImageToImageFilter<CTCImageType,
                                  BinaryImageType>  Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;

  typedef unsigned short                            DistancePixelType;
  typedef itk::Image<DistancePixelType, 3>          DistanceImageType;

  // standard ITK macros
  itkNewMacro(Self);
  itkTypeMacro(SegmentColonWithContrastFilter, itk::ImageToImageFilter);

  void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  

  // ITK shortcuts to get/set m_Threshold
  itkGetMacro(Threshold, CTCPixelType);
  itkSetMacro(Threshold, CTCPixelType);

  // ITK shortcuts to get/set m_MaxSegments
  itkGetMacro(MaxSegments, unsigned int);
  itkSetMacro(MaxSegments, unsigned int);

protected:

  SegmentColonWithContrastFilter();

  typedef itk::ScalarImageKmeansImageFilter
    <  itk::Image<double, 3> > KMeansFilterType;

  typedef itk::MinMaxCurvatureFlowImageFilter<
    itk::Image<double, 3>, itk::Image<double,3> > SmootherType;

  typedef itk::ConnectedThresholdImageFilter
    <BinaryImageType, BinaryImageType> 
    RegionGrowFilterType;

  typedef itk::DanielssonDistanceMapImageFilter
    <BinaryImageType, DistanceImageType>
    DistanceFilterType;

  typedef itk::ResampleImageFilter
    <BinaryImageType, BinaryImageType, double>
    DownsampleFilterType;
  
  void GenerateData();

private:

  SegmentColonWithContrastFilter(Self&);            // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented

  RegionGrowFilterType::Pointer      m_BGRegionGrowFilter;
  DistanceFilterType::Pointer        m_DistanceFilter;
  DownsampleFilterType::Pointer      m_DownsampleFilter;

  long int          m_DownsampleFactor;
  CTCPixelType      m_Threshold;
  unsigned int      m_MaxSegments;
};

} // end namespace ctc

#endif // __ctcSegmentColonWithContrastFilter_h
