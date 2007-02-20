/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcSegmentColonFilter_h
#define __ctcSegmentColonFilter_h

#include "itkSmartPointer.h"
#include "itkImageToImageFilter.h"

#include "itkChangeLabelImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "ctcCTCImage.h"

namespace ctc
{
  
class ITK_EXPORT SegmentColonFilter :
    public itk::ImageToImageFilter<CTCImageType, BinaryImageType>
{
 public:
 
  typedef SegmentColonFilter                        Self;
  typedef itk::ImageToImageFilter<CTCImageType,
                                  BinaryImageType>  Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;

  typedef unsigned short                            DistancePixelType;
  typedef itk::Image<DistancePixelType, 3>          DistanceImageType;

  // standard ITK macros
  itkNewMacro(Self);
  itkTypeMacro(SegmentColonFilter, itk::ImageToImageFilter);

  void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  

  // ITK shortcuts to get/set m_Threshold
  itkGetMacro(Threshold, CTCPixelType);
  itkSetMacro(Threshold, CTCPixelType);

  itkGetMacro(MaxIterations, unsigned int);
  itkSetMacro(MaxIterations, unsigned int);
  itkGetMacro(MinDistanceThreshold, DistancePixelType);
  itkSetMacro(MinDistanceThreshold, DistancePixelType);

protected:

  SegmentColonFilter();
  typedef itk::BinaryThresholdImageFilter
    <CTCImageType, BinaryImageType> 
    ThresholdFilterType;
  
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

  SegmentColonFilter(Self&);            // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented

  ThresholdFilterType::Pointer       m_ThresholdFilter;
  RegionGrowFilterType::Pointer      m_BGRegionGrowFilter;
  DistanceFilterType::Pointer        m_DistanceFilter;
  DownsampleFilterType::Pointer      m_DownsampleFilter;

  long int          m_DownsampleFactor;
  CTCPixelType      m_Threshold;
  CTCPixelType      m_MinPixelValue;
  CTCPixelType      m_MaxPixelValue;
  unsigned int      m_MaxIterations;
  DistancePixelType m_MinDistanceThreshold;

};

} // end namespace ctc

#endif // __ctcSegmentColonFilter_h
