/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcPrincipleCurvatureExtraction_h
#define __ctcPrincipleCurvatureExtraction_h

#include "itkObject.h"
#include "ctcCTCImage.h"
#include "itkListSample.h"

namespace ctc
{
  
  /* PricipleCurvatureExtraction
   *
   *  Given an segmented input image, extract the 
   *  principle curvatures for each point on the surface
   *
   */
  class PrincipleCurvatureExtraction: public itk::Object
  {
  public:
    
    typedef PrincipleCurvatureExtraction              Self;
    typedef itk::SmartPointer<Self>                   Pointer;
    typedef itk::SmartPointer<const Self>             ConstPointer;

    typedef BinaryImageType                           InputImageType;
    typedef itk::Vector< float, 5>                    FeatureType;
    typedef itk::Statistics::ListSample<FeatureType>  FeatureSampleType;
        
    // standard ITK macros
    itkNewMacro(Self);
    itkTypeMacro(PrincipleCurvatureExtraction, itk::Object);
    
    void PrintSelf( std::ostream& os, itk::Indent indent ) const;

    void SetBinaryInput(const BinaryImageType *);

    void SetImageInput(const CTCImageType *);

    void Compute( void );

    FeatureSampleType::Pointer GetOutput( void );

  protected:

    PrincipleCurvatureExtraction();

    ~PrincipleCurvatureExtraction();

  private:

    PrincipleCurvatureExtraction(Self&);            // intentionally not implemented
    void operator=(const Self&);                    // intentionally not implemented

    const CTCImageType *       m_Image;
    const BinaryImageType *    m_ColonImage;
    FeatureSampleType::Pointer m_FeatureVector;
  };



} // end namespace ctc

#endif // __ctcPrincipleCurvatureExtraction_h
