/********************************************************
File: Vertex.hh
Abstract: A floating point vertex class. 
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2002/02/27 20:10:05 $
*********************************************************/

//prevent recursive includes

#ifndef _Vertex_hh
#define _Vertex_hh 1

#include "Data.hh"

#include <list>
#include <vector>

/** Floating point Vertex Class.

A class implementing the storage of 3D verticies. 

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.3 $
*/

namespace IMTK{

class Vertex: public Data
{
private:
	/// vertex location
	float px, py, pz; 

	/// noraml direction
	float nx, ny, nz;

	/// list of triangle indicies (0 based) using this vertex
	std::list<unsigned int> triNeighbors;
	
	int _l; // integer label for this vertex

protected:

public:
	/// Default Constructor
	Vertex();

	/**
	 * Full Constructor
	 * 
	 * @param thePX the point x value
	 * @param thePY the point y value
	 * @param thePZ the point z value
	 * @param theNX the normal x value
	 * @param theNY the normal y value
	 * @param theNZ the normal z value
	 */
	Vertex(float thePX, float thePY, float thePZ, float theNX, float theNY, float theNZ);

	/// Copy Constructor
	Vertex(const Vertex & V);

	/// Destructor
	~Vertex();

	/**
	 * Set Point (inline)
	 * 
	 * @param theX the x value
	 * @param theY the y value
	 * @param theZ the z value
	 */
	void setPoint(float theX, float theY, float theZ){
		px = theX;
		py = theY;
		pz = theZ;
	}

	/**
	 * Set Normal (inline)
	 * 
	 * @param theX the x value
	 * @param theY the y value
	 * @param theZ the z value
	 */
	void setNormal(float theX, float theY, float theZ){
		nx = theX;
		ny = theY;
		nz = theZ;
	}

	/**
	 * Get point x value (inline)
	 * 
	 * @return theX the x value
	 */
	float getPX(){
		return px;
	}

	/**
	 * Get point y value (inline)
	 * 
	 * @return theY the y value
	 */
	float getPY(){
		return py;
	}

	/**
	 * Get point z value (inline)
	 * 
	 * @return theZ the z value
	 */
	float getPZ(){
		return pz;
	}

	/**
	 * Get normal x value (inline)
	 * 
	 * @return theX the x value
	 */
	float getNX(){
		return nx;
	}

	/**
	 * Get normal y value (inline)
	 * 
	 * @return theY the y value
	 */
	float getNY(){
		return ny;
	}

	/**
	 * Get normal z value (inline)
	 * 
	 * @return theZ the z value
	 */
	float getNZ(){
		return nz;
	}

	/**
	 * Overload = (inline)
	 *
	 * assignment invalidates neighborhood
	 */
	 Vertex & operator=(const Vertex & V){

		 if(this == &V) return *this;
		 
		 px = V.px;
		 py = V.py;
		 pz = V.pz;
		 nx = V.nx;
		 ny = V.ny;
		 nz = V.nz;

		 return(*this);
	}

	/**
	 * Overload == (inline)
	 *
	 * verticies are equal if they have the same location 
	 * ignoring the normal
	 */
	int operator==(const Vertex & V){

		return ( (px == V.px) && 
			 (py == V.py) && 
			 (pz == V.pz) );
	}

	/**
	 * Label vertex with interger
	 * 
	 * @param theL the integer label
	 */
	void label(int theL){
		_l = theL;
	}

	/**
	 * Get vertex label
	 * 
	 * @return the integer label
	 */
	int label(){
		return _l;
	}

	/**
	 * Add triangle to neighborhood
	 *
	 * @param index the triangle list index (0 based)
	 */
	void addNeighbor(unsigned int index){

		triNeighbors.push_front(index);
	}

	/**
	 * Get triangle neighborhood indices
	 *
	 */
	void getNeighbors(std::vector<unsigned int> & neighbors);

	/// print vertex info
	void printInfo();

};
}

#endif
