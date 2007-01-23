/********************************************************
File: Volume.hh
Abstract: A general 3 dimensional volume template. 
Created: 06/02/1998
Author: Chris L. Wyatt

Last Revision ($Revision: 1.7 $) 
by $Author: clwyatt $ 
on $Date: 2002/05/16 19:13:15 $
*********************************************************/

//prevent recursive includes
#ifndef _Volume_hh
#define _Volume_hh 1

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

/** 3 dimensional volume template.

A base class template implementing the storage of 3D
grid based data and basic element mathmatics. Data type is
specified when instanciated and must have defined
the appropriate overloads (+ - * / =).

AxialSlice, SagitalSlice, and CoronalSlice are nested classes.
They provide effecient access to the volume data similar to
an iterator.

The bounds, inherited from NDArray, specify volume boundaries in X, Y, Z order.

\begin{verbatim}
    z
   /
  /
 /________ y
 |
 |
 |
 |
 x
\end{verbatim}

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.7 $
*/

namespace IMTK{

template <class T>
class Volume: public NDArray<T,3> 
{
private:
	// coordinate spacing and origin
	float dx, dy, dz; // = to 1 if unknown
	float xorig, yorig, zorig; // = to 0 if unknown

protected:
	
public:

	//forward declarations for slice objects
	class AxialSlice;
	class SagitalSlice;
	class CoronalSlice;

	//make them friendly
	friend class AxialSlice;
	friend class SagitalSlice;
	friend class CoronalSlice;

	// friend classes for efficient access to slices of 3D volume
	/// Axial Slice class
	class AxialSlice{
	private:
		Volume<T> * _parent;
		unsigned int _nrows;
		unsigned int _ncols;
		unsigned int _z;
	public:
		// not very usefull constructor;
		AxialSlice(){}

		AxialSlice(Volume<T> * theP, unsigned int theZ){
			_parent = theP;
			_nrows = _parent->dims[0];
			_ncols = _parent->dims[1];
			assert(theZ < _parent->dims[2]);
			_z = theZ;
		}
		
		///get number of rows
		inline unsigned int nrows(){ return _nrows;}

		//get number of columns
		inline unsigned int ncols(){ return _ncols;}

		///get dimension of row sampling
		inline float drows(){ return _parent->dx;}

		//get dimension of column sampling
		inline float dcols(){ return _parent->dy;}

		//get current slice
		inline unsigned int slice(){ return _z;}

		/// get location
		inline T get(unsigned int theR, unsigned int theC){
			return _parent->getVoxel(theR, theC, _z);
// 			unsigned int index =  _z*(_parent->dims[0])*(_parent->dims[1])
// 				+ theC*(_parent->dims[0]) + theR;
// 			assert((index < _parent->n_));
// 			return *(_parent->d_ + index);
		}
		
		/// set location
		inline void set(unsigned int theR, unsigned int theC, T theVal){
			//_parent->setVoxel(theC, theR, _z, theVal);
			unsigned int index =  _z*(_parent->dims[0])*(_parent->dims[1])
				+ theC*(_parent->dims[0]) + theR;
			assert((index < _parent->n_));
			*(_parent->d_ + index) = theVal;
		}
	};

	/// Sagital Slice class
	class SagitalSlice{
	private:
		Volume<T> * _parent;
		unsigned int _nrows;
		unsigned int _ncols;
		unsigned int _y;
	public:
		// not very usefull constructor;
		SagitalSlice(){}

		SagitalSlice(Volume<T> * theP, unsigned int theY){
			_parent = theP;
			_nrows = _parent->dims[0];
			_ncols = _parent->dims[2];
			assert(theY < _parent->dims[1]);
			_y = theY;
		}
		
		///get number of rows
		inline unsigned int nrows(){ return _nrows;}

		//get number of columns
		inline unsigned int ncols(){ return _ncols;}

		///get dimension of row sampling
		inline float drows(){ return _parent->dx;}

		//get dimension of column sampling
		inline float dcols(){ return _parent->dz;}

		//get current slice
		inline unsigned int slice(){ return _y;}

		/// get location
		inline T get(unsigned int theR, unsigned int theC){
			return _parent->getVoxel(theR, _y, theC);
// 			unsigned int index =  theC*(_parent->dims[0])*(_parent->dims[1])
// 				+ theR*(_parent->dims[1]) + _y;
// 			assert((index < _parent->n_));
// 			return *(_parent->d_ + index);
		}
		
		/// set location
		inline void set(unsigned int theR, unsigned int theC, T theVal){
			unsigned int index =  theC*(_parent->dims[0])*(_parent->dims[1])
				+ theR*(_parent->dims[1]) + _y;
			assert((index < _parent->n_));
			*(_parent->d_ + index) = theVal;
		}
	};

	/// Coronal Slice class
	class CoronalSlice{
	private:
		Volume<T> * _parent;
		unsigned int _nrows;
		unsigned int _ncols;
		unsigned int _x;
	public:
		// not very usefull constructor;
		CoronalSlice(){}

		CoronalSlice(Volume<T> * theP, unsigned int theX){
			_parent = theP;
			_nrows = _parent->dims[2];
			_ncols = _parent->dims[1];
			assert(theX < _parent->dims[0]);
			_x = theX;
		}
		
		///get number of rows
		inline unsigned int nrows(){ return _nrows;}

		//get number of columns
		inline unsigned int ncols(){ return _ncols;}

		///get dimension of row sampling
		inline float drows(){ return _parent->dz;}

		//get dimension of column sampling
		inline float dcols(){ return _parent->dy;}

		//get current slice
		inline unsigned int slice(){ return _x;}

		/// get location
		inline T get(unsigned int theR, unsigned int theC){
			return _parent->getVoxel(_x, theC, theR);
// 			unsigned int index =  theR*(_parent->dims[0])*(_parent->dims[1])
// 				+ _x*(_parent->dims[1]) + theC;
// 			assert((index < _parent->n_));
// 			return *(_parent->d_ + index);
		}
		
		/// set location
		inline void set(unsigned int theR, unsigned int theC, T theVal){
			unsigned int index =  theR*(_parent->dims[0])*(_parent->dims[1])
				+ _x*(_parent->dims[1]) + theC;
			assert((index < _parent->n_));
			*(_parent->d_ + index) = theVal;
		}
	};

	/// Default Constructor
	Volume(): NDArray<T,3>(){

		dx = 1; dy = 1; dz = 1;
		xorig = 0; yorig = 0; zorig = 0;
	}

	/**
	 * Construct Volume with diminsions
	 * 
	 * @param theDims subscript list of dimensions
	 */
	Volume(Subscript<3> theDims): NDArray<T,3>(theDims){}
	
	/**
	 * Construct Volume with diminsions and data
	 * 
	 * @param theDims subscript list of dimensions
	 * @param theDataPtr pointer to array data
	 */
	Volume(Subscript<3> theDims, T * theDataPtr): NDArray<T,3>(theDims, theDataPtr){}

	/// Copy Constructor for NDArray to Volume
	Volume(const NDArray<T,3> & A): NDArray<T,3>(A){}

	/// Copy Constructor for Volume to Volume
	Volume(const Volume<T> & V): NDArray<T,3>(V){}

	/// Destructor
	~Volume(){}
	
	/// get axial slicer
	AxialSlice axial(unsigned int theZ){
		
		assert(theZ < dims[2]);

		return AxialSlice(this, theZ);
	}

	/// get sagital slicer
	SagitalSlice sagital(unsigned int theY){
		
		assert(theY < dims[1]);

		return SagitalSlice(this, theY);
	}

	/// get coroanl slicer
	CoronalSlice coronal(unsigned int theX){
		
		assert(theX < dims[0]);

		return CoronalSlice(this, theX);
	}

	/**
	 * Set value of Volume
	 * 
	 * @param theX
	 * @param theY
	 * @param theZ
	 * @param value value to set location too
	 */
	inline void setVoxel(unsigned int theX, unsigned int theY, unsigned int theZ, T value){

		Subscript<3> sub;

		sub[0] = theX;
		sub[1] = theY;
		sub[2] = theZ;

		this->set(sub, value);
	}

	/**
	 * Return value of Volume
	 * 
	 * @param theX
	 * @param theY
	 * @param theZ
	 * @return value
	 */
	inline T getVoxel(unsigned int theX, unsigned int theY, unsigned int theZ) const{

		Subscript<3> sub;

		sub[0] = theX;
		sub[1] = theY;
		sub[2] = theZ;

		return this->get(sub);
	}

	/**
	 * Get the voxel spacing/sizes
	 * 
	 * @param theDX
	 * @param theDY
	 * @param theDZ
	 */
	inline void getVoxelSpacing(float &theDX, float &theDY, float &theDZ){

		theDX = dx;
		theDY = dy;
		theDZ = dz;
	}

	/**
	 * Set the voxel spacing/sizes
	 * 
	 * @param theDX
	 * @param theDY
	 * @param theDZ
	 */
	inline void setVoxelSpacing(float theDX, float theDY, float theDZ){

		dx = theDX;
		dy = theDY;
		dz = theDZ;
	}

	/**
	 * Get the origin location of voxel (0,0,0)
	 * 
	 * @param theOrigX
	 * @param theOrigY
	 * @param theOrigZ
	 */
	inline void getOrigin(float &theOrigX, float &theOrigY, float &theOrigZ){

		theOrigX = xorig;
		theOrigY = yorig;
		theOrigZ = zorig;
	}

	/**
	 * Set the origin location of voxel (0,0,0)
	 * 
	 * @param theOrigX
	 * @param theOrigY
	 * @param theOrigZ
	 */
	inline void setOrigin(float theOrigX, float theOrigY, float theOrigZ){

		xorig = theOrigX;
		yorig = theOrigY;
		zorig = theOrigZ;
	}

	/**
	 * write the volume data to the given file output stream
	 * 
	 * @param theOutStream
	 */
	bool write(std::ofstream & theOutStream){

		if( theOutStream.bad() ){ //bad stream
			return false;
		}

		// temporary holders
		unsigned long tempul;
		float tempf1, tempf2, tempf3;
		T temptype;

		tempul = dims[0];
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;
		tempul = dims[1];
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;
		tempul = dims[2];
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;

		tempul = sizeof temptype;
		theOutStream.write((char *) &tempul, sizeof tempul);
		if( theOutStream.bad() ) return false;

		getOrigin(tempf1, tempf2, tempf3);
		theOutStream.write((char *) &tempf1, sizeof tempf1);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf2, sizeof tempf2);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf3, sizeof tempf3);
		if( theOutStream.bad() ) return false;

		getVoxelSpacing(tempf1, tempf2, tempf3);
		theOutStream.write((char *) &tempf1, sizeof tempf1);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf2, sizeof tempf2);
		if( theOutStream.bad() ) return false;
		theOutStream.write((char *) &tempf3, sizeof tempf3);
		if( theOutStream.bad() ) return false;

		for(int i = 0; i < size(); i++){
			temptype = (*this)[i];
			theOutStream.write((char *) &temptype, sizeof temptype);
			if( theOutStream.bad() ) return false;
		}

		return true;
	}

	/**
	 * read the volume data from the given file input stream
	 * 
	 * @param theInStream
	 */
	bool read(std::ifstream & theInStream){

		if( theInStream.bad() ){ //bad stream
			return false;
		}

		// temporary holders
		unsigned long tempul;
		float tempf1, tempf2, tempf3;
		T temptype;

		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		dims[0] = tempul;
		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		dims[1] = tempul;
		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		dims[2] = tempul;

		theInStream.read((char *) &tempul, sizeof tempul);
		if( theInStream.bad() ) return false;
		if( tempul != sizeof(temptype) ) return false;

		setBounds(dims);
		clog << "Volume size: " <<  dims[0] << " x " << dims[1] << " x " << dims[2];

		theInStream.read((char *) &tempf1, sizeof tempf1);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf2, sizeof tempf2);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf3, sizeof tempf3);
		if( theInStream.bad() ) return false;
		setOrigin(tempf1, tempf2, tempf3);
		clog << "     origon: " << tempf1 << ", " << tempf2 << ", " << tempf3 << endl;

		theInStream.read((char *) &tempf1, sizeof tempf1);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf2, sizeof tempf2);
		if( theInStream.bad() ) return false;
		theInStream.read((char *) &tempf3, sizeof tempf3);
		if( theInStream.bad() ) return false;
		setVoxelSpacing(tempf1, tempf2, tempf3);
		clog << "       (mm): " << dims[0]*tempf1 << "mm x " << dims[1]*tempf2 << "mm x " << dims[2]*tempf3 << "mm";

		for(int i = 0; i < size(); i++){
			theInStream.read((char *) &temptype, sizeof temptype);
			if( theInStream.bad() ) return false;
			(*this)[i] = temptype;
		}

		return true;
	}

};
}

#endif
