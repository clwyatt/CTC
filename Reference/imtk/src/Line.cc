/********************************************************
File: Line.cc
Abstract: A Line segment class.

See header (Line.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:12 $
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





