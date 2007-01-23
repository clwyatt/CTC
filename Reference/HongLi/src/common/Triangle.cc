/********************************************************
File: Triangle.cc
Abstract: A Triangle class.

See header (Triangle.hh) for documentation

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

#include "Triangle.hh"

using namespace IMTK;

Triangle::Triangle()
{
	p1 = NULL;
	p2 = NULL;
	p3 = NULL;
}

Triangle::Triangle(Vertex * theP1, Vertex * theP2, Vertex * theP3)
{
	p1 = theP1;
	p2 = theP2;
	p3 = theP3;
}

Triangle::Triangle(const Triangle & T)
{
	p1 = T.p1;
	p2 = T.p2;
	p3 = T.p3;
}

Triangle & Triangle::operator=(const Triangle & T)
{
	if(this == &T) return *this;

	p1 = T.p1;
	p2 = T.p2;
	p3 = T.p3;

	return(*this);
}

Triangle::~Triangle()
{

}





