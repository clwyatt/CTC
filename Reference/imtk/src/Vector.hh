/********************************************************
File: Vector.hh
Abstract: A general (numerical) vector template. 
Created: 06/02/1998
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/29 22:52:01 $
*********************************************************/

//prevent recursive includes
#ifndef _Vector_hh
#define _Vector_hh 1

#ifdef IRIX
#include <assert.h>
#include <math.h>
#else
#include <cassert>
#include <cmath>
#endif

#include <iostream>

#include "Data.hh"
#include "NDArray.hh"
#include "Subscript.hh"

/** One dimensional vector template.

A base class template implementing the storage of vector
data and basic element mathmatics. Data type is
specified when instanciated and must have defined
the appropriate overloads (+ - * / =).

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.3 $
*/

namespace IMTK{

template <class T>
class Vector: public NDArray<T,1> 
{
private:

protected:
	
public:

	/// Default Constructor
	Vector(): NDArray<T,1>(){}

	/**
	 * Construct Vector with length
	 * 
	 * @param theDims subscript list of dimensions
	 */
	Vector(Subscript<1> theDims): NDArray<T,1>(theDims){}
	
	/**
	 * Construct Vector with length
	 * 
	 * @param theLength the vector length
	 */
	Vector(int theLength): NDArray<T,1>(Subscript<1>(&theLength)){}

	/**
	 * Construct Vector with length and data
	 * 
	 * @param theDims subscript list of dimensions
	 * @param theDataPtr pointer to array data
	 */
	Vector(Subscript<1> theDims, T * theDataPtr): NDArray<T,1>(theDims, theDataPtr){}

	/// Copy Constructor for NDArray to Vector
	Vector(const NDArray<T,1> & A): NDArray<T,1>(A){}

	/// Copy Constructor for Vector to Vector
	Vector(const Vector<T> & V): NDArray<T,1>(V){}

	/// Destructor
	~Vector(){}

	/**
	 * Overload [] operator
	 * 
	 * @param theI
	 */
	inline T& operator[](unsigned int theI){

		assert(theI < n_);
		return *(d_ + theI); 
	}

	/**
	 * Overload const [] operator
	 * 
	 * @param theI
	 */
	inline const T& operator[](unsigned int theI) const{
		
		assert(theI < n_);
		return *(d_ + theI); 
	}

	/**
	 * Set value of Vector
	 * 
	 * @param theI
	 * @param value value to set location too
	 */
	inline void setElement(unsigned int theI, T value){

		unsigned int index = theI;

		assert((index < n_));

		*(d_ + index) = value;
	}

	/**
	 * Return value of Vector
	 * 
	 * @param theI
	 * @return value
	 */
	inline T getElement(unsigned int theI){

   		unsigned int index = theI;

		assert((index < n_));

		return *(d_ + index);
	}
};
}

#endif
