/********************************************************
File: LF3Dfull.hh
Abstract:  An implementation of a 3D level function.
Created: 11/21/2000 from a number of previous attempts
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

//prevent recursive includes

#ifndef _LF3Dfull_hh
#define _LF3Dfull_hh 1

#include <slist>

#include "Volume.hh"
#include "Vector.hh"

/** LF3Dfull Class.

A class implementing the storage of 3D level function
using the full matrix approach.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version  $Revision$
*/

class LF3Dfull: public IMTK::Volume<float>
{
	typedef std::slist< IMTK::Subscript<3> >::iterator tubeIterator;

private:
	// inner and outer tube diameters
	float innerR, outerR;
	
	// current tagged inner points
	std::slist< IMTK::Subscript<3> > inner;
	
	// current tagged outer points (land mines)
	std::slist< IMTK::Subscript<3> > outer;
	
public:
	
	/// Constructor
	LF3Dfull(): IMTK::Volume<float>(){
		innerR = 2.5;
		outerR = 5.0;
	};
	
	/// Custom Constructor
	LF3Dfull(IMTK::Subscript<3> bounds): IMTK::Volume<float>(bounds){
		innerR = 2.5;
		outerR = 5.0;
	};
	
	/// Copy Constructor
	LF3Dfull(const LF3Dfull & LF): IMTK::Volume<float>(LF){
		innerR = LF.innerR;
		outerR = LF.outerR;
	};
	
	/// Destructor
	~LF3Dfull(){};
	
	/// inner iterator begin
	tubeIterator innerBegin(){
		return inner.begin();
	};
 
	/// inner iterator end
	tubeIterator innerEnd(){
		return inner.end();
	};

	/**
	 * set inner tube radius
	 * 
	 * @param theR the radius
	 */
	void setInnerR(float theR){

		innerR = theR;
	};

	/**
	 * get inner tube radius
	 * 
	 * @return the radius
	 */
	float getInnerR(void) const{

		return innerR;
	};

	/**
	 * get number of inner tube points
	 * 
	 * @return the number of points
	 */
	int getNumInner(void) const{

		return inner.size();
	};


	/**
	 * add voxel to inner tube list
	 * 
	 * @param x
	 * @param y
	 * @parma z
	 */
	void addInner(int x, int y, int z){

		IMTK::Subscript<3> loc;
		
		loc[0] = x; loc[1] = y; loc[2] = z;
		
		inner.push_front(loc);
	};

	/**
	 * clear the inner tube list
	 * 
	 */
	void clearInner(void){
		
		inner.clear();
	};

	/// outer iterator begin
	tubeIterator outerBegin(){
		return outer.begin();
	};
 
	/// outer iterator end
	tubeIterator outerEnd(){
		return outer.end();
	};

	/**
	 * set outer tube radius
	 * 
	 * @param theR the radius
	 */
	void setOuterR(float theR){

		outerR = theR;
	};

	/**
	 * get outer tube radius
	 * 
	 * @return the radius
	 */
	float getOuterR(void) const{

		return outerR;
	};

	/**
	 * get number of outer tube points
	 * 
	 * @return the number of points
	 */
	int getNumOuter(void) const{

		return outer.size();
	};

	/**
	 * add voxel to outer tube list
	 * 
	 * @param x
	 * @param y
	 * @parma z
	 */
	void addOuter(int x, int y, int z){

		IMTK::Subscript<3> loc;
		
		loc[0] = x; loc[1] = y; loc[2] = z;
		
		outer.push_front(loc);
	};

	/**
	 * clear the inner tube list
	 * 
	 */
	void clearOuter(void){
		
		outer.clear();
	};
};

#endif
