/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include "ctcCenterlineSpatialObjectPoint.h"

namespace ctc 
{
  /** Constructor */
  CenterlineSpatialObjectPoint::CenterlineSpatialObjectPoint() 
  : Superclass()
  { 
    m_Medialness = 0;
  }

  /** Destructor */
  CenterlineSpatialObjectPoint::~CenterlineSpatialObjectPoint() 
  {
  }

  float CenterlineSpatialObjectPoint::GetMedialness() const 
  {
    return m_Medialness;
  }

  void CenterlineSpatialObjectPoint::SetMedialness( const float newMedialness ) 
  {
    m_Medialness = newMedialness;
  }

  void CenterlineSpatialObjectPoint::PrintSelf( std::ostream & os, 
						itk::Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    os << indent << "Medialness: " << m_Medialness << std::endl;
  }


  CenterlineSpatialObjectPoint::Self & 
  CenterlineSpatialObjectPoint
  ::operator=(const CenterlineSpatialObjectPoint & rhs) 
  {
    this->m_R = rhs.m_R;
    m_Medialness = rhs.m_Medialness;
    this->m_NumDimensions = rhs.m_NumDimensions;
    this->m_T = rhs.m_T;
    this->m_Normal1 = rhs.m_Normal1;
    this->m_Normal2 = rhs.m_Normal2;

    return * this;
  }

} // end namespace ctc

