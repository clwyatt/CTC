/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcDetectAndSegmentPolypsNappi02Filter_h
#define __ctcDetectAndSegmentPolypsNappi02Filter_h

#include "itkSmartPointer.h"
#include "itkImageToImageFilter.h"

#include "ctcCTCImage.h"

namespace ctc
{
  
 class ITK_EXPORT DetectAndSegmentPolypsNappi02Filter :
  public itk::ImageToImageFilter<BinaryImageType, BinaryImageType>
    {
    public:
 
      typedef DetectAndSegmentPolypsNappi02Filter     Self;
      typedef itk::ImageToImageFilter<BinaryImageType,
	                              BinaryImageType>  Superclass;
      typedef itk::SmartPointer<Self>                   Pointer;
      typedef itk::SmartPointer<const Self>             ConstPointer;

      // standard ITK macros
      itkNewMacro(Self);
      itkTypeMacro(DetectAndSegmentPolypsNappi02Filter, 
                   itk::ImageToImageFilter);

      void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  
      void SetImageInput(const CTCImageType *);

    protected:

      DetectAndSegmentPolypsNappi02Filter();
      //~DetectAndSegmentPolypsNappi02Filter();

      void GenerateData();

    private:

      // intentionally not implemented
      DetectAndSegmentPolypsNappi02Filter(Self&);            
      void operator=(const Self&);         
      
      void BuildWallRegion();

      const CTCImageType * m_Image;
    };
  
} // end namespace ctc

#endif // __ctcDetectAndSegmentPolypsNappi02Filter_h
