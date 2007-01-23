/********************************************************
File: RaggedArray.hh
Abstract: A 2D ragged array template. 
Created: 06/02/1998
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

//prevent recursive includes
#ifndef _RaggedArray_hh
#define _RaggedArray_hh 1

#ifdef IRIX
#include <assert.h>
#include <math.h>
#else
#include <cassert>
#include <cmath>
#endif

#include "Data.hh"

/** 2D ragged array template.

A base class template implementing the storage of a list of
(possible different length) arrays.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

template <class T>
class RaggedArray: public Data 
{
private:

	T ** d_;
	unsigned int rowlength, *collengths;
	
public:
	/// Default Constructor
	RaggedArray(){
		d_ = NULL;
		rowlength = 0;
		collengths = NULL;
	}

	/**
	 * Construct RaggedArray with specified number of rows
	 * 
	 * @param nRows the number of rows
	 */
	RaggedArray(unsigned int nRows){

		rowlength = nRows;
		d_ = new T*[rowlength];
		collengths = new unsigned int[rowlength];
		for(unsigned int i=0; i < rowlength; i++){
			collengths[i] = 0;
			d_[i] = NULL;
		}
	}
	
	/// Copy Constructor
	RaggedArray(const RaggedArray<T> & A){

		rowlength = A.rowlength;

		if(rowlength > 0){
			d_ = new T*[rowlength];
			for(int i=0; i < rowlength; i++){
				collengths[i] = A.collengths[i];
				if(collengths[i] > 0){
					d_[i] = new T[collengths[i]];
					for(int j=0; j < collengths[i]; j++)
						d_[i][j] = A.d_[i][j];
				}
				else{
					di_[i] = NULL;
				}
			}
		}
		else{
			collengths = NULL;
			d_ = NULL;
		}
	}

	/// Destructor
	~RaggedArray(){
		for(unsigned int i=0; i < rowlength; i++){
			delete [] d_[i];
		}
		delete [] collengths;
	}
	
	/**
	 * insert a row into the ragged array (deletes old row if present)
	 * 
	 * @param theRow the row to insert at
	 * @param theLength the length of the new row
	 * @param theArray pointer to first element of the array to be inserted
	 */
	void insertRow(unsigned int theRow, unsigned int theLength, T* theArray){

		assert(theRow < rowlength);

		if(collengths[theRow] != 0) delete [] d_[theRow];
		collengths[theRow] = theLength;
		d_[theRow] = theArray;
	}

	/**
	 * append a row into the end of the ragged array
	 * 
	 * @param theLength the length of the new row
	 * @param theArray pointer to first element of the array to be inserted
	 */
	void appendRow(unsigned int theLength, T* theArray){

		T** tempd_;
		unsigned int * tempcollengths;

		if(rowlength == 0){
			rowlength = 1;
			d_ = new T*[rowlength];
			collengths = new unsigned int[rowlength];
		}
		else{
			rowlength += 1;
			tempd_ = new T*[rowlength];
			tempcollengths = new unsigned int[rowlength];
			for(unsigned int i=0; i < rowlength-1; i++){
				tempd_[i] = d_[i];
				tempcollengths[i] = collengths[i];
			}
			delete [] d_;
			delete [] collengths;
			d_ = tempd_;
			collengths = tempcollengths;
		}

		collengths[rowlength-1] = theLength;
		d_[rowlength-1] = theArray;
	}

	/**
	 * Overload [] operator
	 * 
	 * @param theI
	 */
	inline T* operator[](unsigned int theI){

		assert(theI < rowlength);
		return d_[theI]; 
	}

	/**
	 * Overload const [] operator
	 * 
	 * @param theI
	 */
	inline const T* operator[](unsigned int theI) const{
		
		assert(theI < n_);
		return d_[theI]; 
	}


	/**
	 * Get number of rows 
	 * 
	 */
	inline unsigned int size() const {return rowlength;}

	/**
	 * Get number of columns in row 
	 * 
	 * @param theRow the row number
	 */
	inline unsigned int size(unsigned int theRow) const {
		assert(theRow < rowlength);
		return collengths[theRow];
	}


	/**
	 * Set value of row, column
	 * 
	 * @param theRow the row 
	 * @param theCol the column
	 */
	inline void set(unsigned int theRow, unsigned int theCol, const T &theValue){

		assert(theRow < rowlengths);
		assert(theCol < collengths[theRow]);

		d_[theRow][theCol]  = theValue;
	}

	/**
	 * Get value of row, column
	 * 
	 * @param theRow the row 
	 * @param theCol the column
	 */
	inline T get(unsigned int theRow, unsigned int theCol){

		assert(theRow < rowlengths);
		assert(theCol < collengths[theRow]);

		return d_[theRow][theCol];
	}

};
}

#endif
