/********************************************************
File: Vertex.cc
Abstract: A general (numerical) vector template.

See header (Vertex.hh) for documentation

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

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
}

Vertex::Vertex(float thePX, float thePY, float thePZ, float theNX, float theNY, float theNZ)
{
	px = thePX;
	py = thePY;
	pz = thePZ;

	nx = theNX;
	ny = theNY;
	nz = theNZ;
}

Vertex::Vertex(const Vertex & V)
{
	px = V.px;
	py = V.py;
	pz = V.pz;

	nx = V.nx;
	ny = V.ny;
	nz = V.nz;
}

Vertex::~Vertex()
{

}

void Vertex::getNeighbors(std::vector<unsigned int> & neighbors)
{

	neighbors.resize(triNeighbors.size());

	std::slist<unsigned int>::iterator it = triNeighbors.begin();
	int i = 0;
	while( it != triNeighbors.end() ){
		neighbors[i] = *it;
		i+= 1;
		it++;
	}
}

void Vertex::printInfo()
{
	cerr << "Point: " << px << ", " << py << ", " << pz << "\n";
	cerr << "Normal: " << nx << ", " << ny << ", " << nz << "\n";
	cerr << triNeighbors.size() << " Neighbors. Indices ";
	std::slist<unsigned int>::iterator it = triNeighbors.begin();
	while( it != triNeighbors.end() ){
		cerr << *it << " ";
		it++;
	}
	cerr << "\n\n" << endl;
}



