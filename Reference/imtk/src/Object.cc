/********************************************************
File: Object.cc
Abstract: Skeleton immplementation for Object

See header (Object.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:14 $
*********************************************************/

#include "Object.hh"


IMTK::Object::Object(void){

	refCount = 1;
}

IMTK::Object::Object(const Object & O){

	refCount = 1;
}

IMTK::Object::~Object(void){

}

void IMTK::Object::addReference(){

	refCount += 1;
}

void IMTK::Object::unReference(){

	refCount -= 1;

	if(refCount <= 0)
		delete this;
}


