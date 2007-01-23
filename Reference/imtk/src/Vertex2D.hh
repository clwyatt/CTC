/********************************************************
File: Vertex2D.hh
Abstract: A floating point vertex class. 
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:18 $
*********************************************************/

//prevent recursive includes

#ifndef _Vertex2D_hh
#define _Vertex2D_hh 1

#include "Data.hh"

/** Floating point 2D Vertex Class.

A class implementing the storage of 2D verticies. 

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.2 $
*/

namespace IMTK{

class Vertex2D: public Data
{
private:
	float px, py;
	
	int _l; // integer label for this vertex

protected:

public:
	/// Default Constructor
	Vertex2D();

	/**
	 * Full Constructor
	 * 
	 * @param theX the x value
	 * @param theY the y value
	 */
	Vertex2D(float theX, float theY);

	/// Copy Constructor
	Vertex2D(const Vertex2D & V);

	/// Destructor
	~Vertex2D();

	/**
	 * Set Point (inline)
	 * 
	 * @param theX the x value
	 * @param theY the y value
	 */
	void set(float theX, float theY){
		px = theX;
		py = theY;
	}

	/**
	 * Get x value (inline)
	 * 
	 * @return theX the x value
	 */
	float getX(){
		return px;
	}

	/**
	 * Get y value (inline)
	 * 
	 * @return theY the y value
	 */
	float getY(){
		return py;
	}

	/**
	 * Overload == (inline)
	 *
	 */
	int operator==(const Vertex2D & V){

		return ( (px == V.px) && 
			 (py == V.py) );
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

};
}

#endif
