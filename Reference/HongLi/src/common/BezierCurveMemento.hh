/********************************************************
File: BezierCurveMemento.hh
Abstract: A class to store the internal state  of B-spline Curve class. 
Created: 05/22/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:32 $
*********************************************************/

//prevent recursive includes

#ifndef _Bezier_Curve_Memento_hh
#define _Bezier_Curve_Memento_hh 1

#include <cassert>

#include "BezierPoint.hh"

/** Bezier Curve Memento.

A class implementing a memento (see the design patterns book)
storing the internal state of the BezierCurve class. This is
usefull for implementation of a undo/redo stack. 

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

// forward declare the BezierCurve class
class BezierCurve;

class BezierCurveMemento: public Data
{
	friend class BezierCurve;

private:
	// copy of BezierCurve private data members 
	unsigned int _numberOfSegments;
	bool _closed;
	list<BezierPoint> plist;

protected:

	void getState(unsigned int& theNumOfSeg, bool& isClosed, list<BezierPoint>& theList) const{

		theNumOfSeg = _numberOfSegments;
		isClosed = _closed;
		theList = plist;
	};
	
	void setState(unsigned int theNumOfSeg, bool isClosed, const list<BezierPoint>& theList){
		
		_numberOfSegments = theNumOfSeg;
		_closed = isClosed;
		plist = theList;
	};
	
public:
	/// Default Constructor
	BezierCurveMemento(){
		_numberOfSegments = 0;
		_closed = false;
	}
	
	/// Copy Constructor
	BezierCurveMemento(const BezierCurveMemento & BC){
		_numberOfSegments = BC._numberOfSegments;
		_closed = BC._closed;
		plist = BC.plist;
	}
	
	/// Destructor
	~BezierCurveMemento(){};
};
}

#endif
