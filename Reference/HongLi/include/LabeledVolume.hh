/********************************************************
File: LabeledVolume.hh
Abstract: A Labeled 3 dimensional volume template. 
Created: 09/03/2002
Author: Hong Li

Last Revision ($Revision: 1.2 $) 
by $Author: hongli $ 
on $Date: 2002/12/19 22:25:31 $
*********************************************************/

//prevent recursive includes
#ifndef _LabeledVolume_hh
#define _LabeledVolume_hh 1

#ifdef IRIX
#include <assert.h>
#include <math.h>
#else
#include <cassert>
#include <cmath>
#endif
#include "Volume.hh"


template <class T>
class LabeledVolume : public IMTK::Volume <T>
{
private :
	char * theLabel;

public :
	inline void AllocLabels()
	{
	theLabel = new char[size()];
	assert(theLabel);
	for (unsigned int i =0; i<size(); i++)
		*(theLabel + i) = 0;		
	}	

	inline void DeAllocLabels()
	{
	if(theLabel) delete [] theLabel;
	}	

	~LabeledVolume() {
	if(theLabel) delete [] theLabel;
	}
	/**
	 * Set label of Volume
	 * 
	 * @param theX
	 * @param theY
	 * @param theZ
	 * @param value value to set location too
	 */
	inline void Label(unsigned int theX, unsigned int theY, unsigned int
	theZ, char value){

		IMTK::Subscript<3> sub;

		sub[0] = theX;
		sub[1] = theY;
		sub[2] = theZ;

		unsigned int index = sub.getIndex(dims);
		*(theLabel + index) = value;
	}

	/**
	 * Return label of Volume
	 * 
	 * @param theX
	 * @param theY
	 * @param theZ
	 * @return value
	 */
	inline char Label(unsigned int theX, unsigned int theY, unsigned int theZ) const{

		IMTK::Subscript<3> sub;

		sub[0] = theX;
		sub[1] = theY;
		sub[2] = theZ;

   		unsigned int index = sub.getIndex(dims);

		assert((index >= 0)&&(index < n_));

		return *(theLabel + index);
	}
};

#endif
