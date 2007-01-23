/********************************************************
File: BezierPoint.hh
Abstract: A class to store B-spline Points. 
Created: 05/22/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/05/26 18:19:42 $
*********************************************************/

//prevent recursive includes

#ifndef _BezierPoint_hh
#define _BezierPoint_hh 1

#include <cassert>

#include "Vertex.hh"

/** Bezier point Class.

A class implementing the storage of Bezier anchor and control verticies. 

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.2 $
*/

namespace IMTK{

class BezierPoint: public Data
{
private:
	float ax, ay, az;    // anchor location
	float c1x, c1y, c1z; // control point 1 position
	float c2x, c2y, c2z; // control point 2 position

protected:

public:
	/// Default Constructor
	BezierPoint(){
		ax = 0.0f; ay = 0.0f; az = 0.0f;
		c1x = 0.0f; c1y = 0.0f; c1z = 0.0f;
		c2x = 0.0f; c2y = 0.0f; c2z = 0.0f;
	}

	/// Copy Constructor
	BezierPoint(const BezierPoint & BP){
		ax = BP.ax; ay = BP.ay; az = BP.az;
		c1x = BP.c1x; c1y = BP.c1y; c1z = BP.c1z;
		c2x = BP.c2x; c2y = BP.c2y; c2z = BP.c2z;
	}

	/// Destructor
	~BezierPoint(){};

	/**
	 * overload ==
	 * two Bezier points are equal if the have the same anchor
	 */
	bool operator==(const BezierPoint& bp) const {
		return ( (ax == bp.ax) && (ay == bp.ay) && (az == bp.az) );
	}

	/**
	 * get anchor position
	 * 
	 * @param theX 
	 * @param theY 
	 * @param theZ 
	 */
	void getAnchor(float &theX, float &theY, float &theZ) const{
		theX = ax; theY = ay; theZ = az;
	}

	/**
	 * set anchor position
	 * 
	 * @param theX 
	 * @param theY 
	 * @param theZ 
	 */
	void setAnchor(float theX, float theY, float theZ){
		ax = theX; ay = theY; az = theZ;
	}

	/**
	 * get control 1 position
	 * 
	 * @param theX 
	 * @param theY 
	 * @param theZ 
	 */
	void getControl1(float &theX, float &theY, float &theZ) const{
		theX = c1x; theY = c1y; theZ = c1z;
	}

	/**
	 * set control 1 position
	 * 
	 * @param theX 
	 * @param theY 
	 * @param theZ 
	 */
	void setControl1(float theX, float theY, float theZ){
		c1x = theX; c1y = theY; c1z = theZ;
	}

	/**
	 * get control 2 position
	 * 
	 * @param theX 
	 * @param theY 
	 * @param theZ 
	 */
	void getControl2(float &theX, float &theY, float &theZ) const{
		theX = c2x; theY = c2y; theZ = c2z;
	}

	/**
	 * set control 2 position
	 * 
	 * @param theX 
	 * @param theY 
	 * @param theZ 
	 */
	void setControl2(float theX, float theY, float theZ){
		c2x = theX; c2y = theY; c2z = theZ;
	}

};
}

#endif
