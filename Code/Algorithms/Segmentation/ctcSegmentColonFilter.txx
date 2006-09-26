/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#ifndef _ctcSegmentColonFilter_txx
#define _ctcSegmentColonFilter_txx

#include "ctcSegmentColonFilter.h"

namespace ctc
{

template <class TImageType>
SegmentColonFilter<TImageType>
::SegmentColonFilter()
{
  m_ChangeLabelFilter = ChangeLabelFilterType::New();

  m_Threshold = 1;

}

template <class TImageType>
void
SegmentColonFilter<TImageType>::
GenerateData()
{
  m_ChangeLabelFilter->SetInput( this->GetInput() );

  m_ChangeLabelFilter->Update();

  this->GraftOutput( m_ChangeLabelFilter->GetOutput() );
}

template <class TImageType>
void
SegmentColonFilter<TImageType>::
PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os
    << indent << "Segment Colon Threshold:" << this->m_Threshold
    << std::endl;
}

}

#endif // _ctcSegmentColonFilter_txx