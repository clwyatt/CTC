/********************************************************
File: Object.cc
Abstract: Skeleton immplementation for Object

See header (Object.hh) for documentation

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
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


