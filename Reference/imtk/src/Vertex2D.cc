/********************************************************
File: Vertex2D.cc
Abstract: A 2D vertex class.

See header (Vertex2D.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:18 $
*********************************************************/

#include "Vertex2D.hh"

using namespace IMTK;

Vertex2D::Vertex2D()
{
	px = 0.0;
	py = 0.0;
}

Vertex2D::Vertex2D(float theX, float theY)
{
	px = theX;
	py = theY;
}

Vertex2D::Vertex2D(const Vertex2D & V)
{
	px = V.px;
	py = V.py;
}

Vertex2D::~Vertex2D()
{

}





