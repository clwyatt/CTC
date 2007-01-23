/********************************************************
File: Vertex2D.cc
Abstract: A 2D vertex class.

See header (Vertex2D.hh) for documentation

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:34 $
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





