/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcGaborImageFilter_h
#define __ctcGaborImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkPixelTraits.h"
#include "itkCommand.h"


namespace ctc
{

/** \class GaborImageFilter
 * \brief Computes the self-similar wavelet (Gabor) response
 * 
 */
template <typename TInputImage, 
          typename TOutputImage= TInputImage >
class ITK_EXPORT GaborImageFilter:
    public itk::ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef GaborImageFilter  Self;
  typedef itk::ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;
  
  
  /** Pixel Type of the input image */
  typedef TInputImage                                    InputImageType;
  typedef TOutputImage                                   OutputImageType;
  typedef typename TInputImage::PixelType                PixelType;
  typedef typename itk::NumericTraits<PixelType>::RealType    RealType;
  typedef typename itk::NumericTraits<PixelType>::ScalarRealType ScalarRealType;


  /** Image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Define the image type for internal computations 
      RealType is usually 'double' in NumericTraits. 
      Here we prefer float in order to save memory.  */

  typedef typename itk::NumericTraits< PixelType >::FloatType InternalRealType;
  typedef itk::Image<InternalRealType,  
                itkGetStaticConstMacro(ImageDimension) >   RealImageType;

  typedef typename itk::Vector<InternalRealType,
     itkGetStaticConstMacro(ImageDimension) > VectorType;
  typedef itk::Image< VectorType,
		      itkGetStaticConstMacro(ImageDimension) > VectorImageType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** sigma access, in the units of image spacing.  */
  itkSetMacro( Sigma, ScalarRealType );
  itkGetMacro( Sigma, ScalarRealType );

  /** wave vector access*/
  itkSetMacro( Wave, VectorType );
  itkGetMacro( Wave, VectorType );

protected:
  GaborImageFilter();
  virtual ~GaborImageFilter() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );

  // Override since the filter produces the entire dataset ??????
  // void EnlargeOutputRequestedRegion(DataObject *output); ?????

private:
 //purposely not implemented
  GaborImageFilter(const Self&);
  void operator=(const Self&);
 
  /** scale */
  ScalarRealType m_Sigma;
  VectorType m_Wave;

};

} // end namespace ctc

#ifndef ITK_MANUAL_INSTANTIATION
#include "ctcGaborImageFilter.txx"
#endif

#endif // __ctcGaborImageFilter_h
