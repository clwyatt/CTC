/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/

#ifndef __ctcCenterlineSpatialObjectPoint_h
#define __ctcCenterlineSpatialObjectPoint_h

#include "itkTubeSpatialObjectPoint.h"

namespace ctc
{

/** \class CenterlineSpatialObjectPoint
* \brief Point used for a centerline tube definition
*
* This class contains all the functions necessary to define a point
* that can be used to build colon centerline tubes.
*
* \sa CenterlineSpatialObject 
*/ 
  class CenterlineSpatialObjectPoint: public itk::TubeSpatialObjectPoint<3>
{

public:

  typedef CenterlineSpatialObjectPoint                 Self;
  typedef itk::TubeSpatialObjectPoint<3>               Superclass;
  typedef itk::Point< double, 3 >                      PointType;
 
  /** Constructor. */
  CenterlineSpatialObjectPoint( void );

  /** Default destructor. */
  virtual ~CenterlineSpatialObjectPoint( void );

  /** Get Medialness */
  float GetMedialness( void ) const;

  /** Set Medialness */
  void SetMedialness(const float newMedialness);

  /** Copy one CenterlineSpatialObjectPoint to another */
  Self & operator=(const CenterlineSpatialObjectPoint & rhs);

protected:

  /** The medialness of the tube point */
  float m_Medialness;

  /** Print the object */
  void PrintSelf( std::ostream & os, itk::Indent indent) const;
};

} // end of namespace ctc

#endif // __ctcCenterlineSpatialObjectPoint_h
