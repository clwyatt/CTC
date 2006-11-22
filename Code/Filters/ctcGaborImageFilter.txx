/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#ifndef _ctcGaborImageFilter_txx
#define _ctcGaborImageFilter_txx

#include "ctcGaborImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkProgressAccumulator.h"

namespace ctc
{


/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage >
GaborImageFilter<TInputImage,TOutputImage>
::GaborImageFilter()
{

  // default sigma
  m_Sigma = 1.0;

  // default wave vector
  for( unsigned int i = 0; i<ImageDimension-1; i++ )
    {
      m_Wave[i] = 1.0;
    }

}

/**
 * Filter the image
 */
template <typename TInputImage, typename TOutputImage >
void
GaborImageFilter<TInputImage,TOutputImage >
::GenerateData(void)
{

  itkDebugMacro(<< "GaborImageFilter generating data ");

}


template <typename TInputImage, typename TOutputImage>
void
GaborImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "Sigma: " << m_Sigma << std::endl;
  os << "Wave vector: " << m_Wave << std::endl;
}


} // end namespace ctc

#endif //_ctcGaborImageFilter_txx
