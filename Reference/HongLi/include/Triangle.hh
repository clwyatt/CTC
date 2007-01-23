/********************************************************
File: Triangle.hh
Abstract: A Triangle class. 
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:29 $
*********************************************************/

//prevent recursive includes

#ifndef _Triangle_hh
#define _Triangle_hh 1

#include "Data.hh"
#include "Vertex.hh"

/** Triangle Class.

A class implementing the storage of a triangle using
pointers to verticies.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

class Triangle: public Data
{
private:
	/** pointers to verticies */
	Vertex *p1, *p2, *p3;
                             
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
	Triangle(Vertex * theP1, Vertex * theP2, Vertex * theP3);

	/// Copy constructor
	Triangle(const Triangle & T);

	/// assignement opertator
	Triangle & operator=(const Triangle & T);

	/// Destructor
	~Triangle();

	/**
	 * Set the verticies of the triangle (inline)
	 * 
	 * @param theP1 the first vertex
	 * @param theP2 the second vertex
	 * @param theP3 the third vertex
	 */
	void set(Vertex * theP1, Vertex * theP2, Vertex * theP3){
		p1 = theP1;
		p2 = theP2;
		p3 = theP3;
	}

	/**
	 * Get the first vertex (inline)
	 * 
	 * @return theP1 the first vertex
	 */
	 Vertex * getV1(){
		return p1;
	}

	/**
	 * Get the second vertex (inline)
	 * 
	 * @return theP2 the second vertex
	 */
	 Vertex * getV2(){
		return p2;
	}

	/**
	 * Get the third vertex (inline)
	 * 
	 * @return theP3 the third vertex
	 */
	 Vertex * getV3(){
		return p3;
	}
};
}

#endif
