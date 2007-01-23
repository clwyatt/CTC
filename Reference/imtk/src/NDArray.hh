/********************************************************
File: NDArray.hh
Abstract: A general N dimensional array template. 
Created: 06/02/1998
Author: Chris L. Wyatt

Last Revision ($Revision: 1.6 $) 
by $Author: clwyatt $ 
on $Date: 2002/04/17 20:44:32 $
*********************************************************/

//prevent recursive includes
#ifndef _NDArray_hh
#define _NDArray_hh 1

#ifdef IRIX
#include <assert.h>
#include <math.h>
#else
#include <cassert>
#include <cmath>
#endif

#include <fstream>

#include "Data.hh"
#include "Subscript.hh"

/** N dimensional array template.

A base class template implementing the storage of multidimensional
grid based data and basic element mathmatics. Data type is
specified when instanciated and must have defined
the appropriate overloads (+ - * / =).

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.6 $
*/

namespace IMTK{

template <class T, int N>
class NDArray: public Data 
{
private:

protected:
	// list of dimension sizes
	Subscript<N> dims;

	// pointer to data
	T *d_;
	
	// size of array
	unsigned int n_;
	
public:
	typedef T* pointer;
	typedef T* iterator;
	typedef T&  reference;
	typedef const  T*  const_iterator;
	typedef const  T&  const_reference;

	/// Default Constructor
	NDArray(){
		d_ = NULL;
		n_ = 0;
	}

	/**
	 * Construct NDArray with diminsions
	 * 
	 * @param theDims subscript list of dimensions
	 */
	NDArray(Subscript<N> theDims){
		int i, size;
		
		d_ = NULL;
		dims = theDims;
		
		size = 1;
		for(i=0; i < N; i++){
			size = size*dims[i];
		}

		d_ = new T[size];
		n_ = size;
	}
	
	/**
	 * Construct NDArray with diminsions and data
	 * 
	 * @param theDims subscript list of dimensions
	 * @param theDataPtr pointer to array data
	 */
	NDArray(Subscript<N> theDims, T * theDataPtr){
		int i, size;

		dims = theDims;

		size = 1;
		for(i=0; i < N; i++){
			size = size*dims[i];
		}

		d_ = theDataPtr;
		n_ = size;
	}

	/// Copy Constructor
	NDArray(const NDArray<T,N> & A){
		int i, size;

		d_ = NULL;
		dims = A.dims;
		
		size = 1;
		for(i=0; i < N; i++){
			size = size*dims[i];
		}
		
		d_ = new T[size];
		n_ = size;

		for(i = 0; i < size; i++)
			*(d_ + i) = *(A.d_ + i);
	}

	/// Destructor
	~NDArray(){delete [] d_;}
	
	/// Access Methods
	iterator begin() {return d_;}
	iterator end()   {return d_ + n_;}
	const iterator begin() const {return d_;}
	const iterator end() const  {return d_ + n_;}
	
	/// Overload +
	NDArray & operator+(const NDArray<T,N> & A){

		int i;
		NDArray<T,N> * result;
	       
		result = new NDArray<T,N>(this->dims);

		for(i=0; i < n_; i++){
			result->d_[i] = d_[i] + A.d_[i]; 
		}

		return(*result);
	}

	/// Overload -
	NDArray & operator-(const NDArray<T,N> & A){

		int i;
		NDArray<T,N> * result;
	       
		result = new NDArray<T,N>(this->dims);

		for(i=0; i < n_; i++){
			result->d_[i] = d_[i] - A.d_[i]; 
		}

		return(*result);
	}

	/// Overload *
	NDArray & operator*(const NDArray<T,N> & A){

		int i;
		NDArray<T,N> * result;
	       
		result = new NDArray<T,N>(this->dims);

		for(i=0; i < n_; i++){
			result->d_[i] = d_[i] * A.d_[i]; 
		}

		return(*result);
	}

	/// Overload /
	NDArray & operator/(const NDArray<T,N> & A){

		int i;
		NDArray<T,N> * result;
	       
		result = new NDArray<T,N>(this->dims);

		for(i=0; i < n_; i++){
			result->d_[i] = d_[i] / A.d_[i]; 
		}

		return(*result);
	}

 
	/// Overload =
	NDArray & operator=(const NDArray<T,N> & A){

		int i;

		if(this == &A) return *this;

		setBounds(A.getBounds());

		for(i=0; i < n_; i++){
			d_[i] = A.d_[i]; 
		}

		return(*this);
	}

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
	 * Fill NDArray with value
	 * 
	 * @param theValue the value to fill with
	 */
	void fill(const T theValue){
		unsigned int i;
		
		for(i=0; i < n_; i++){
			d_[i] = theValue; 
		}
	}

	/**
	 * Set bounds of NDArray 
	 * 
	 * @param theBounds the bounds to set
	 */
	inline void setBounds(Subscript<N> theBounds){
		int i;
		unsigned int size;
		
		size = 1;
		for(i=0; i < N; i++){
			size = size*theBounds[i];
		}

		if(size != n_){
			delete [] d_;
			d_ = new T[size];
			n_ = size;
		}

		dims = theBounds;
	}

	/**
	 * Get bounds of NDArray 
	 * 
	 */
	inline Subscript<N> getBounds() const {return dims;}


	/**
	 * Set value of NDArray at subscript
	 * using right-handed coordinate system
	 * 
	 * @param theSub the location to set
	 * @param value value to set location too
	 */
	inline void set(const Subscript<N> & theSub, const T value){

		unsigned int index = theSub.getIndex(dims);

		assert(index >= 0);

		*(d_ + index) = value;
	}

	/**
	 * Return value of NDArray at subscript
	 * using right-handed coordinate system
	 *
	 * @param theSub the location to return
	 * @return value
	 */
	inline T get(const Subscript<N> & theSub) const{

   		unsigned int index = theSub.getIndex(dims);

		//nasty hack to force bus error if can't catch abort properly in debugger
		//if((index < 0)||(index >= n_)){ delete ((void *) 1);}

		assert((index >= 0)&&(index < n_));

		return *(d_ + index);
	}

	/**
	 * Return max of NDArray over all elements
	 *
	 * @return max
	 */
	T max(){
		T m = d_[0];
		for(int i=1; i < n_; i++){
			if(d_[i] > m) m = d_[i];
		}
		return m;
	}

	/**
	 * Return max of NDArray over all elements
	 *
	 * @return max
	 */
	T min(){
		T m = d_[0];
		for(int i=1; i < n_; i++){
			if(d_[i] < m) m = d_[i];
		}
		return m;
	}

	/**
	 * Return size of array in multiples of type T
	 *
	 * @return size
	 */
	inline unsigned int size() const{

		return n_;
	}

	/**
	 * Dump the data to raw file
	 *
	 * @param theFile pointer to file string
	 */
	void dump(char * theFile){
		std::ofstream  outfile;

		outfile.open( theFile );
		outfile.write((char *)d_, sizeof(T)*n_);
		outfile.close();
	}

};
}

#endif
