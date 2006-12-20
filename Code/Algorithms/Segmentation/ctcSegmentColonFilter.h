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


namespace ctc
{

  template <class TInputImageType, class TOutputImageType>
class ITK_EXPORT SegmentColonFilter :
    public itk::ImageToImageFilter<TInputImageType, TOutputImageType>
{
public:

  typedef SegmentColonFilter                   Self;
  typedef itk::ImageToImageFilter<TInputImageType,TOutputImageType> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  typedef unsigned char BinaryPixelType;
  typedef itk::Image<BinaryPixelType, 3> BinaryImageType; 
  
  typedef unsigned short DistancePixelType;
  typedef itk::Image<DistancePixelType, 3> DistanceImageType;

  // standard ITK macros
  itkNewMacro(Self);
  itkTypeMacro(SegmentColonFilter, itk::ImageToImageFilter);

  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  typedef typename TInputImageType::PixelType InputPixelType;

  // ITK shortcuts to get/set m_Threshold
  itkGetMacro(Threshold, InputPixelType);
  itkSetMacro(Threshold, InputPixelType);

  itkGetMacro(MaxIterations, unsigned int);
  itkSetMacro(MaxIterations, unsigned int);
  itkGetMacro(MinDistanceThreshold, DistancePixelType);
  itkSetMacro(MinDistanceThreshold, DistancePixelType);

protected:

  SegmentColonFilter();

protected:

  typedef itk::ChangeLabelImageFilter< TInputImageType, TInputImageType > 
    ChangeLabelFilterType;
  
  typedef itk::BinaryThresholdImageFilter<TInputImageType, BinaryImageType> 
    ThresholdFilterType;
  
  typedef itk::ConnectedThresholdImageFilter<BinaryImageType, BinaryImageType> 
    RegionGrowFilterType;

  typedef itk::DanielssonDistanceMapImageFilter<BinaryImageType, DistanceImageType>
    DistanceFilterType;

  typedef itk::ResampleImageFilter<BinaryImageType, BinaryImageType, double>
    DownsampleFilterType;

  void GenerateData();

private:

  signed short m_constShortROSMarker;
  long int downsampleFactor;

  SegmentColonFilter(Self&);   // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented


  typename ChangeLabelFilterType::Pointer     m_ChangeLabelFilter;
  typename ThresholdFilterType::Pointer       m_ThresholdFilter;
  typename RegionGrowFilterType::Pointer      m_BGRegionGrowFilter;
  typename DistanceFilterType::Pointer        m_DistanceFilter;
  typename DownsampleFilterType::Pointer      m_DownsampleFilter;

  InputPixelType m_Threshold;
  InputPixelType m_MinPixelValue;
  InputPixelType m_MaxPixelValue;
  unsigned int m_MaxIterations;
  DistancePixelType m_MinDistanceThreshold;

};

} // end namespace ctc

#ifndef ITK_MANUAL_INSTANTIATION
#include "ctcSegmentColonFilter.txx"
#endif

#endif // __ctcSegmentColonFilter_h
