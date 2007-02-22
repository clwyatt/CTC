/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcRawVolumeExtraction_h
#define __ctcRawVolumeExtraction_h

#include "itkSmartPointer.h"
#include "itkImageToImageFilter.h"

#include "itkCropImageFilter.h"

#include "ctcCTCImage.h"

namespace ctc
{
  
/* RawVolumeExtraction
 *
 *  Given and input image and DICOM coordinate, extract
 *  a cubic region with sides 2*CropSize, oriented in the
 *  normal direction at scale Sigma.
 *
 */
class ITK_EXPORT RawVolumeExtraction :
    public itk::ImageToImageFilter<CTCImageType, CTCImageType>
{
 public:
 
  typedef RawVolumeExtraction                        Self;
  typedef itk::ImageToImageFilter<CTCImageType,
                                  CTCImageType>     Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;

  typedef unsigned short                            DistancePixelType;
  typedef itk::Image<DistancePixelType, 3>          DistanceImageType;

  // standard ITK macros
  itkNewMacro(Self);
  itkTypeMacro(RawVolumeExtraction, itk::ImageToImageFilter);

  void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  

  //get/set m_DICOMCoordinate
  itk::Point<double, 3> GetDICOMCoordinate()
    {
      return m_DICOMCoordinate;
    };

  void SetDICOMCoordinate(const itk::Point<double, 3> & coord)
    {
      // check in bounds here ??
      m_DICOMCoordinate = coord;
    };

  // ITK shortcuts to get/set m_CropSize
  itkGetMacro(CropSize, double);
  itkSetMacro(CropSize, double);
  
  // ITK shortcuts to get/set m_Sigma
  itkGetMacro(Sigma, double);
  itkSetMacro(Sigma, double);

protected:

  RawVolumeExtraction();

  void GenerateData();

  typedef itk::CropImageFilter< ctc::CTCImageType, ctc::CTCImageType >
    FilterType;

private:

  RawVolumeExtraction(Self&);            // intentionally not implemented
  void operator=(const Self&);          // intentionally not implemented

  // center coordinate
  itk::Point<double, 3> m_DICOMCoordinate;

  // crop box size
  double m_CropSize;

  // scale to compute derivatives
  double m_Sigma;

  // internal filters
  FilterType::Pointer m_CropFilter;

};

} // end namespace ctc

#endif // __ctcRawVolumeExtraction_h
