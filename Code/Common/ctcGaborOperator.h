/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcGaborOperator_h
#define __ctcGaborOperator_h

#include "itkExceptionObject.h"
#include "itkNeighborhoodOperator.h"

namespace ctc {

/**
 * \class GaborOperator
 *
 * \brief A NeighborhoodOperator for performing a Gabor
 * filter operation * at a pixel location.
 * 
 * The current implementation of the Sobel operator is for 2 and 3 dimensions 
 * only. The ND version is planned for future releases.
 */
template<class TPixel, unsigned int VDimension=2,
  class TAllocator = NeighborhoodAllocator<TPixel> >
class ITK_EXPORT SobelOperator
  : public NeighborhoodOperator<TPixel, VDimension, TAllocator>
{
public:
  /** Standard typedefs */
  typedef SobelOperator Self;
  typedef NeighborhoodOperator<TPixel, VDimension, TAllocator>  Superclass;

  itkTypeMacro(SobelOperator, NeighborhoodOperator);
  
  SobelOperator() {}
  SobelOperator(const Self& other)
    : NeighborhoodOperator<TPixel, VDimension, TAllocator>(other) 
  {  }

  /** Creates the operator with length only in the specified direction.  For
   * the Sobel operator, this 
   * The radius of the operator will be 0 except along the axis on which
   * the operator will work.
   * \sa CreateToRadius \sa FillCenteredDirectional \sa SetDirection() \sa GetDirection() */
  virtual void CreateDirectional()
  {
    this->CreateToRadius(1);
  }
  
  /** Creates the operator with a specified radius ("square", same length
   * on each side). The spatial location of the coefficients within the
   * operator is defined by the subclass implementation of the Fill method.
   * \sa CreateDirectional \sa Fill */
  // virtual void CreateToRadius(const unsigned long);
  /**
   * Assignment operator
   */
  Self &operator=(const Self& other)
  {
    Superclass::operator=(other);
    return *this;
  }
  /**
   * Prints some debugging information
   */
  virtual void PrintSelf(std::ostream &os, Indent i) const  
  { 
    os << i << "SobelOperator { this=" << this  << "}" << std::endl;
    Superclass::PrintSelf(os, i.GetNextIndent());
  }

protected:
  /**
   * Typedef support for coefficient vector type.  Necessary to
   * work around compiler bug on VC++.
   */
  typedef typename Superclass::CoefficientVector CoefficientVector;
  typedef typename Superclass::PixelType PixelType;

  /**
   * Calculates operator coefficients.
   */
  CoefficientVector GenerateCoefficients();

  /**
   * Arranges coefficients spatially in the memory buffer.
   */
  void Fill(const CoefficientVector &c);

};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSobelOperator.txx"
#endif

#endif


