/********************************************************
File: Triangle.cc
Abstract: A Triangle class.

See header (Triangle.hh) for documentation

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:31 $
*********************************************************/

#include "Triangle_with_Features.hh"

Triangle::Triangle()
{
	p1 = NULL;
	p2 = NULL;
	p3 = NULL;
}

Triangle::Triangle(Vertex_with_Features * theP1, Vertex_with_Features * theP2, Vertex_with_Features * theP3)
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

Triangle::~Triangle()
{

}

float Triangle::getArea()
{
  	float x1, y1, z1;
	float x2, y2, z2; 
	float x,y,z;

	x1 = p1->getPX()-p2->getPX();
	x2 = p3->getPX()-p2->getPX();
	
	y1 = p1->getPY()-p2->getPY();
	y2 = p3->getPY()-p2->getPY();
	
	z1 = p1->getPZ()-p2->getPZ();
	z2 = p3->getPZ()-p2->getPZ();

      	x = y1 * z2 - z1 * y2;
      	y = z1 * x2 - x1 * z2;
       	z = x1 * y2 - y1 * x2;
	
	return (0.5*sqrt(x*x+y*y+z*z));
	
}

mvVec3f Triangle::getNormDir()
{
  	float x1, y1, z1;
	float x2, y2, z2; 
	float x,y,z;

	x1 = p2->getPX()-p1->getPX();
	x2 = p3->getPX()-p2->getPX();
	
	y1 = p2->getPY()-p1->getPY();
	y2 = p3->getPY()-p2->getPY();
	
	z1 = p2->getPZ()-p1->getPZ();
	z2 = p3->getPZ()-p2->getPZ();

      	x = y1 * z2 - z1 * y2;
      	y = z1 * x2 - x1 * z2;
       	z = x1 * y2 - y1 * x2;
	mvVec3f n(x,y,z);
	n.normalize();
	return (n);
	
}




