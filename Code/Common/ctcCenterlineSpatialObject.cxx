/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#include "ctcCenterlineSpatialObject.h" 

namespace ctc  
{ 

/** Constructor */
CenterlineSpatialObject::CenterlineSpatialObject() : Superclass()
{ 
  this->m_ParentPoint = -1;
  this->SetDimension(3);
  this->SetTypeName("CenterlineSpatialObject");
} 
 
/** Destructor */
CenterlineSpatialObject::~CenterlineSpatialObject()
{ 
} 

/** Print the object */ 
void CenterlineSpatialObject::PrintSelf( std::ostream& os, 
					 itk::Indent indent ) const 
{ 
  os << indent << "CenterlineSpatialObject(" << this << ")" << std::endl; 
  Superclass::PrintSelf( os, indent ); 
}

} // end namespace ctc 


