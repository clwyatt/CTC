/********************************************************
File: Vertex.cc
Abstract: A general (numerical) vector template.

See header (Vertex.hh) for documentation

Last Revision ($Revision: 1.4 $) 
by $Author: clwyatt $ 
on $Date: 2002/05/16 19:11:50 $
*********************************************************/
#include <iostream>

#include "Vertex.hh"

using namespace IMTK;

Vertex::Vertex()
{
	px = 0.0;
	py = 0.0;
	pz = 0.0;
	
	nx = 0.0;
	ny = 0.0;
	nz = 0.0;

	_l = -1;
}

Vertex::Vertex(float thePX, float thePY, float thePZ, float theNX, float theNY, float theNZ)
{
	px = thePX;
	py = thePY;
	pz = thePZ;

	nx = theNX;
	ny = theNY;
	nz = theNZ;

	_l = -1;
}

Vertex::Vertex(const Vertex & V)
{
	px = V.px;
	py = V.py;
	pz = V.pz;

	nx = V.nx;
	ny = V.ny;
	nz = V.nz;

	_l = -1;
}

Vertex::~Vertex()
{

}

void Vertex::getNeighbors(std::vector<unsigned int> & neighbors)
{

	neighbors.resize(triNeighbors.size());

	std::list<unsigned int>::iterator it = triNeighbors.begin();
	int i = 0;
	while( it != triNeighbors.end() ){
		neighbors[i] = *it;
		i+= 1;
		it++;
	}
}

void Vertex::printInfo()
{
	std::cerr << "Point: " << px << ", " << py << ", " << pz << "\n";
	std::cerr << "Normal: " << nx << ", " << ny << ", " << nz << "\n";
	std::cerr << triNeighbors.size() << " Neighbors. Indices ";
	std::list<unsigned int>::iterator it = triNeighbors.begin();
	while( it != triNeighbors.end() ){
		std::cerr << *it << " ";
		it++;
	}
	std::cerr << "\n\n" << std::endl;
}



