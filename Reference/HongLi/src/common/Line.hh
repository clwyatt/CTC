/********************************************************
File: Line.hh
Abstract: A Line class. 
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

//prevent recursive includes

#ifndef _Line_hh
#define _Line_hh 1

#include "Data.hh"
#include "Vertex2D.hh"

/** Line Class.

A class implementing the storage of a line segment using
pointers to verticies.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

class Line: public Data
{
private:
	/** pointers to verticies */
	Vertex2D *p1, *p2;

protected:

public:
	/// Default Constructor
	Line();

	/**
	 * Full Constructor
	 * 
	 * @param theP1 the first vertex
	 * @param theP2 the second vertex
	 * @param theP3 the third vertex
	 */
	Line(Vertex2D * theP1, Vertex2D * theP2);

	// Copy constructor
	Line(const Line & L);

	/// Destructor
	~Line();

	/**
	 * Set the verticies of the line segment (inline)
	 * 
	 * @param theP1 the first vertex
	 * @param theP2 the second vertex
	 */
	void set(Vertex2D * theP1, Vertex2D * theP2){
		p1 = theP1;
		p2 = theP2;
	}

	/**
	 * Get the first vertex (inline)
	 * 
	 * @return theP1 the first vertex
	 */
	 Vertex2D * getV1(){
		return p1;
	}

	/**
	 * Get the second vertex (inline)
	 * 
	 * @return theP2 the second vertex
	 */
	 Vertex2D * getV2(){
		return p2;
	}
};
}

#endif
