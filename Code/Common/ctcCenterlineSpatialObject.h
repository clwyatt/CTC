/*******************************************************************************
Program:   Wake Forest University - Virginia Tech CTC Software
Id:        $Id$
Language:  C++
*******************************************************************************/
#ifndef __ctcCenterlineSpatialObject_h
#define __ctcCenterlineSpatialObject_h

#include "itkTubeSpatialObject.h"
#include "ctcCenterlineSpatialObjectPoint.h"

namespace ctc 
{

/** 
* \class CenterlineSpatialObject
* \brief Representation of a centeline based on the tube spatial object classes.
*
* The centeline is a tube defined by a set of points. Each tube can
* be connected to a tube network, by using the AddSpatialObject() methods
* of a CenterlineSpatialObject Object. 
* A tube is also identified by an id number when connected to a network.
*
* \sa CenterlineSpatialObjectPoint 
*/
  
class CenterlineSpatialObject 
  :public itk::TubeSpatialObject< 3, CenterlineSpatialObjectPoint >
{

public:

  typedef CenterlineSpatialObject                      Self;
  typedef itk::TubeSpatialObject< 3, CenterlineSpatialObjectPoint > Superclass;
  typedef itk::SmartPointer < Self >                        Pointer;
  typedef itk::SmartPointer < const Self >                  ConstPointer;
  typedef CenterlineSpatialObjectPoint                 TubePointType;
  typedef Superclass::PointListType           PointListType;
  typedef Superclass::PointType               PointType;
  typedef Superclass::TransformType           TransformType;
  typedef Superclass::SpatialObjectPointType  SpatialObjectPointType;
  typedef itk::VectorContainer<unsigned long,PointType> PointContainerType;
  typedef itk::SmartPointer<PointContainerType>         PointContainerPointer;
  typedef Superclass::VectorType              VectorType;
  typedef Superclass::CovariantVectorType     CovariantVectorType;
  typedef Superclass::BoundingBoxType         BoundingBoxType;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Method for creation through the object factory. */
  itkTypeMacro( CenterlineSpatialObject, TubeSpatialObject );
    
protected:

  CenterlineSpatialObject();
  virtual ~CenterlineSpatialObject();

  /** Method to print the object.*/
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  
};

} // end namespace ctc

#endif // __ctcCenterlineSpatialObject_h
