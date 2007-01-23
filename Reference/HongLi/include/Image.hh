/********************************************************
File: Image.hh
Abstract: A general 2 dimensional image template. 
Created: 06/02/1998
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:28 $
*********************************************************/

//prevent recursive includes
#ifndef _Image_hh
#define _Image_hh 1

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

/** 2 dimensional image template.

A base class template implementing the storage of 2D
grid based data and basic element mathmatics. Data type is
specified when instanciated and must have defined
the appropriate overloads (+ - * / =).

NOTE: A left-handed coordinate system is used to get and set pixel
locations (notice this differs from the default NDArray get/set).
The bounds, inherited from NDArray, specify volume boundaries in X, Y order.

\begin{verbatim}
 _________ y
 |
 |
 |
 |
 x
\end{verbatim}

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

template <class T>
class Image: public NDArray<T,2> 
{
private:

protected:
	
public:

	/// Default Constructor
	Image(): NDArray<T,2>(){}

	/**
	 * Construct Image with diminsions
	 * 
	 * @param theDims subscript list of dimensions
	 */
	Image(Subscript<2> theDims): NDArray<T,2>(theDims){}
	
	/**
	 * Construct Image with diminsions and data
	 * 
	 * @param theDims subscript list of dimensions
	 * @param theDataPtr pointer to array data
	 */
	Image(Subscript<2> theDims, T * theDataPtr): NDArray<T,2>(theDims, theDataPtr){}

	/// Copy Constructor for NDArray to Image
	Image(const NDArray<T,2> & A): NDArray<T,2>(A){}

	/// Copy Constructor for Image to Image
	Image(const Image<T> & I): NDArray<T,2>(I){}

	/// Destructor
	~Image(){}
	
	/**
	 * Set value of Image
	 * 
	 * @param theX
	 * @param theY
	 * @param value value to set location too
	 */
	inline void setPixel(unsigned int theX, unsigned int theY, T value){

		Subscript<2> sub;

		sub[0] = theX;
		sub[1] = theY;

		this->set(sub, value);
	}

	/**
	 * Return value of Image
	 * 
	 * @param theX
	 * @param theY
	 * @return value
	 */
	inline T getPixel(unsigned int theX, unsigned int theY) const{

		Subscript<2> sub;

		sub[0] = theX;
		sub[1] = theY;

		return this->get(sub);
	}
};
}

#endif
