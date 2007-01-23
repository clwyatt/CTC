/********************************************************
File: BezierCurve.hh
Abstract:  A Bezier curve class.
Created: 06/01/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:32 $
*********************************************************/

//prevent recursive includes

#ifndef _BezierCurve_hh
#define _BezierCurve_hh 1

#include <list>
#include <math.h>

#include "Data.hh"
#include "BezierPoint.hh"
#include "BezierCurveMemento.hh"

/** Bezier Curve Class.

A class implementing the storage of (closed) 3D
Bezier Curves.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version  $Revision: 1.1.1.1 $
*/

namespace IMTK{

class BezierCurve: public Data
{
private:
	unsigned int _numberOfSegments;
	bool _closed;
	list<BezierPoint> plist; // list of control and anchor points

	//static const float pickRadius = 2.0;
	static const float pickRadius = 0.1;
	

protected:

public:

	/// Default Constructor
	BezierCurve();

	/// Copy Constructor (inline)
	BezierCurve(const BezierCurve & C){
		// copy not implemented
		assert(0);
	}

	/** Destructor
	 * For efficiency reasons, the vertices are deleted when
	 * the list is deleted.
	 */
	~BezierCurve();

	/**
	 * get bezier point
	 * 
	 */
	BezierPoint getAnchor(int anchorID);

	/**
	 * add segment
	 * 
	 */
	void addSegment(float x, float y, float z);

	/**
	 * insert segment
	 * 
	 */
	void insertSegment(float x, float y, float z);

	/**
	 * select segment anchor
	 * 
	 */
	bool selectAnchor(float x, float y, float z, int & anchorID);

	/**
	 * select segment control
	 * 
	 */
	bool selectControl(float x, float y, float z, int anchorID, int & controlN);

	/**
	 * update anchor position
	 * 
	 */
	void updateAnchor(float x, float y, float z, int anchorID);

	/**
	 * delete anchor
	 * 
	 */
	void deleteAnchor(int anchorID);

	/**
	 * update control position
	 * 
	 */
	void updateControl(float x, float y, float z, int anchorID, int controlN);

	/**
	 * Get number of segments
	 * 
	 * @return number of Bezier segments
	 */
	unsigned int getNsegments();

	/**
	 * Get point array for rendering 
	 * Bezier segment in OpenGL 
	 */
	void getPoints(unsigned int segment, float* ctrlpoints);

	/**
	 * is the curve empty
	 */
	bool isEmpty();

	/**
	 * is the curve closed
	 */
	bool isClosed();

	/**
	 * modify the Curve the that given by the memento
	 */
	void setMemento(const BezierCurveMemento& M);

	/**
	 * return a memento capturing the current curve
	 */
	 BezierCurveMemento* createMemento();

};
}

#endif
