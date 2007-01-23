/********************************************************
File: Triangle.hh
Abstract: A Triangle class. 
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:29 $
*********************************************************/

#ifndef _Triangle_with_Features_hh
#define _Triangle_with_Features_hh 1

/*in order to avoid the Triangle definition conflict with imtk lib */
#define Triangle Triangle_with_Features

#include "Data.hh"
#include "Vertex_with_Features.hh"
#include "mvVec3f.hh"

/** Triangle Class with features.

A class implementing the storage of a triangle using
pointers to verticies and pointers to neighboring triangles.

some features are added to the original class Triangle, written by Chris
Think of a sub class for it
*/

class Triangle: public IMTK::Data
{
private:
	/** pointers to verticies */
	Vertex_with_Features *p1, *p2, *p3;

	/** pointers to neighboring triangles
	 *  n1 adjacent to edge p1-p2, n2, adjacent to edge p2-p3, etc 
	 */
	Triangle *n1, *n2, *n3;
	int _l;
	mvVec3f norm;
	int Index;
                             
protected:

public:
	/// Default Constructor
	Triangle();

	/**
	 * Full Constructor
	 * 
	 * @param theP1 the first vertex
	 * @param theP2 the second vertex
	 * @param theP3 the third vertex
	 */
	Triangle(Vertex_with_Features * theP1, Vertex_with_Features * theP2, Vertex_with_Features * theP3);

	// Copy constructor
	Triangle(const Triangle & T);

	/// Destructor
	~Triangle();

	/**
	 * Set the verticies of the triangle (inline)
	 * 
	 * @param theP1 the first vertex
	 * @param theP2 the second vertex
	 * @param theP3 the third vertex
	 */
	void set(Vertex_with_Features * theP1, Vertex_with_Features * theP2, Vertex_with_Features * theP3){
		p1 = theP1;
		p2 = theP2;
		p3 = theP3;
	}

	/**
	 * Get the first vertex (inline)
	 * 
	 * @return theP1 the first vertex
	 */
	 Vertex_with_Features * getV1(){
		return p1;
	}

	/**
	 * Get the second vertex (inline)
	 * 
	 * @return theP2 the second vertex
	 */
	 Vertex_with_Features * getV2(){
		return p2;
	}

	/**
	 * Get the third vertex (inline)
	 * 
	 * @return theP3 the third vertex
	 */
	 Vertex_with_Features * getV3(){
		return p3;
	}
	
	float getArea();
	mvVec3f getNormDir();
	/**
	 * Label Triangle with interger
	 * 
	 * @param theL the integer label
	 */
	void label(int theL){
		_l = theL;
	}

	/**
	 * Get Triangle label
	 * 
	 * @return the integer label
	 */
	int label(){
		return _l;
	}
	
	/**
	 * Temporarily Label Triangle with interger
	 * 
	 * @param theL the integer Index
	 */
	void setIndex(int theL){
		Index = theL;
	}

	/**
	 * Get Triangle label
	 * 
	 * @return the integer Index
	 */
	int getIndex(){
		return Index;
	}
	
};

#endif
