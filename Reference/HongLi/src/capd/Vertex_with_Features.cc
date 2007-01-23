#include "Vertex_with_Features.hh"

///default constructor
Vertex_with_Features::Vertex_with_Features()
{
normValid = false;
flag = false;
}

Vertex_with_Features::Vertex_with_Features( Vertex_with_Features &V) :Vertex( V)
{
neighborTriangleLabels = V.getNeighborTriangleLabels();
ptNeighborTriangles = V.getNeighborTriangles();
}

///deconstructor
Vertex_with_Features::~Vertex_with_Features()
{}
void Vertex_with_Features::addNeighborTriangleLabel(const int label) 
{ 
	neighborTriangleLabels.push_front(label);
}		
void Vertex_with_Features::addNeighborTriangle(void * pt) 
{ 
	ptNeighborTriangles.push_front(pt);
}		
