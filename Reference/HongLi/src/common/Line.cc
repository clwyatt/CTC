/********************************************************
File: Line.cc
Abstract: A Line segment class.

See header (Line.hh) for documentation

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

#include "Line.hh"

using namespace IMTK;

Line::Line()
{
	p1 = NULL;
	p2 = NULL;
}

Line::Line(Vertex2D * theP1, Vertex2D * theP2)
{
	p1 = theP1;
	p2 = theP2;
}

Line::Line(const Line & L)
{
	p1 = L.p1;
	p2 = L.p2;
}

Line::~Line()
{

}





