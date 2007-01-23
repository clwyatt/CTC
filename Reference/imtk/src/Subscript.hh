/********************************************************
File: Subscript.hh
Abstract: A general N dimensional array subscript. 
Created: 10/24/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/29 22:52:01 $
*********************************************************/

//prevent recursive includes
#ifndef _Subscript_hh
#define _Subscript_hh 1

#ifdef IRIX
#include <assert.h>
#include <stdlib.h>
#else
#include <cassert>
#include <cstdlib>
#endif

#ifndef _IMTK_SUBSCRIPT_TYPE
#define _IMTK_SUBSCRIPT_TYPE unsigned int
#endif

/** N dimensional array subscript.

Class for storing N dimensional subsctipt into an N dimensional array.
Define _IMTK_SUBSCRIPT_TYPE at compile or use default of unsigned int.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.3 $
*/

namespace IMTK{

template <int N>
class Subscript 
{
private:
	// subscript elements
	_IMTK_SUBSCRIPT_TYPE element[N];

	// store number of elements
	int size;

public:
	/// Default Constructor
	Subscript(){
		size = N;
		for(int i=0; i<N; i++)
			element[i] = 0;
	}

	/// Constructor
	Subscript(int *theElement){
		size = N;
		for(int i=0; i<N; i++)
			element[i] = theElement[i];
	}

	///Copy Constructor
	Subscript(const Subscript<N> & S){
		
		for(int i=0; i<N; i++)
			element[i] = S.element[i];
	}

	inline _IMTK_SUBSCRIPT_TYPE& operator[](int i){
		
		assert(i >= 0);
		assert(i < N) ;

		return element[i]; 
	}

	inline const _IMTK_SUBSCRIPT_TYPE& operator[](int i) const{

		assert(i >= 0);
		assert(i < N) ;

		return element[i]; 
	}

	/**
	 * Overload equality
	 * 
	 */
	bool operator==(const Subscript<N> &right){

		bool flag = true;
		unsigned int i = 0;

		while( flag && (i < N)){
			flag = element[i] == right.element[i];
			i++;
		}

		return flag;
	}

	/**
	 * Return linear index into array given bounding box
	 * using right-handed coordinate system
	 * 
	 * @return theBounds the size of the array
	 */
	int getIndex(const Subscript<N> &theBounds) const{
		int i, j, size, loc;
			
		loc = 0;
		for(i=0; i < N; i++){
			assert(theBounds[i] > 0);

			if(element[i] < theBounds[i]){
				size = 1;
				for(j=0; j < i; j++){
					size = size*theBounds[j];
				}
				loc = loc + element[i]*size;
			}
			else{
				loc = -1;
				return loc;
			}    
		}
		return loc;
	}

	/**
	 * set subcript corresponding to given
	 * index into given bounding box using
	 * right-handed coordinate system
	 * 
	 * @param theBounds the size of the array
	 * @param theIndex the linear index
	 */
	 void setSubscript(const Subscript<N> &theBounds, const unsigned int &theIndex){
		int i, j, size, loc;
			
		loc = theIndex;
		for(i=N-1; i >= 0; i--){
			size = 1;
			for(j=0; j < i; j++){
				size = size*theBounds[j];
			}

			element[i] = (unsigned int)( ((float)loc) / ((float)size) );

			loc = loc - element[i]*size;
		}
	}

};
}

#endif
