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

namespace ctc
{

template <class TImageType>
class ITK_EXPORT SegmentColonFilter :
    public itk::ImageToImageFilter<TImageType, TImageType>
{
public:

  typedef SegmentColonFilter                   Self;
  typedef itk::ImageToImageFilter<TImageType,TImageType> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  // standard ITK macros
  itkNewMacro(Self);
  itkTypeMacro(SegmentColonFilter, itk::ImageToImageFilter);

  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  typedef typename TImageType::PixelType PixelType;

  // ITK shortcuts to get/set m_Threshold
  itkGetMacro( Threshold, PixelType);
  itkSetMacro( Threshold, PixelType);

protected:

  SegmentColonFilter();

protected:

  typedef itk::ChangeLabelImageFilter< TImageType, TImageType > ChangeLabelFilterType;

  void GenerateData();

private:

  SegmentColonFilter(Self&);   // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented


  typename ChangeLabelFilterType::Pointer     m_ChangeLabelFilter;

  PixelType m_Threshold;
};

} // end namespace ctc

#ifndef ITK_MANUAL_INSTANTIATION
#include "ctcSegmentColonFilter.txx"
#endif

#endif // __ctcSegmentColonFilter_h
