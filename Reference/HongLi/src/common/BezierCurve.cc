/********************************************************
File: BezierCurve.cc
Abstract: implementation for BezierCurve

See header (BezierCurve.hh) for documentation

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:32 $
*********************************************************/

#include "BezierCurve.hh"

using namespace IMTK;


BezierCurve::BezierCurve()
{
	_numberOfSegments = 0;
	_closed = false;
};

BezierCurve::~BezierCurve()
{

};

BezierPoint BezierCurve::getAnchor(int anchorID)
{
	int id = 0;

	list<BezierPoint>::iterator it = plist.begin();
	while((it != plist.end()) && (id != anchorID)){
		id++;
		it++;
	}
	
	return *it;
};

void BezierCurve::addSegment(float x, float y, float z)
{
	list<BezierPoint>::iterator curr, prev;
	BezierPoint newbp;
	float newx, newy, newz, dx, dy, dz, mag;
	float xp, yp, zp;
	bool closingCurve = false;

	if(plist.empty()){
		newbp.setAnchor(x, y, z);
		newbp.setControl1(x-1, y, z); //arbitrary initial control locations
		newbp.setControl2(x+1, y, z);
		plist.push_back(newbp);
	}
	else{
		// test if new point is with pickRadius for first point
		curr = plist.begin();
		curr->getAnchor(newx, newy, newz);
		dx = x - newx;
		dy = y - newy;
		dz = z - newz;
		mag = sqrt(dx*dx + dy*dy + dz*dz);
		//clog << "Mag = " << mag << endl;
		if(mag < pickRadius){
			closingCurve = true;
			
			// cannot close curve less than 3 points, do nothing
			if(_numberOfSegments < 2){
				clog << "BezierCurve: Warning cannot close curve with less than 3 Anchor points." << endl;
				return;
			}
		}
		else{
			newx = x;
			newy = y;
			newz = z;
		}

		// compute the (unit) vector between prev and new point
		prev = --plist.end();
		prev->getAnchor(xp, yp, zp);
		dx = newx - xp;
		dy = newy - yp;
		dz = newz - zp;
		mag = sqrt(dx*dx + dy*dy + dz*dz);
		//clog << "Mag = " << mag << endl;
		if(mag < pickRadius){
			clog << "BezierCurve: Warning Anchor point within pickRadius of non-terminating point" << endl;
		}
		// update previous control point
		prev->setControl1(xp + 0.25*dx, yp + 0.25*dy, zp + 0.25*dz);

		// add next anchor point or update first=last control point
		if(!closingCurve){
			newbp.setAnchor(newx, newy, newz);
			newbp.setControl1(xp + 1.25*dx, yp + 1.25*dy, zp + 1.25*dz);
			newbp.setControl2(xp + 0.75*dx, yp + 0.75*dy, zp + 0.75*dz);
			plist.push_back(newbp);
		}
		else{
			curr->setControl2(xp + 0.75*dx, yp + 0.75*dy, zp + 0.75*dz);
			_closed = true;
		}

		_numberOfSegments += 1;
	}
};

void BezierCurve::insertSegment(float x, float y, float z)
{
	float dx, dy, dz, dist, mindist;
	float x1, x2, y1, y2, z1, z2;
	float mx1, mx2, my1, my2, mz1, mz2;
	bool minFound = false;
	bool insertAtEnd = false;
	list<BezierPoint>::iterator insertpos;
	BezierPoint newbp;

	// do nothing if curve is not closed
	if(!_closed) return;

	mindist = 1e10; // arbitrarily large number
	
	//loop through finding likely anchors to perform insert between
	list<BezierPoint>::iterator it = plist.begin();
	while(it != plist.end()){
		it->getAnchor(x1, y1, z1);
		dx = x - x1; dy = y - y1, dz = z - z1;
		dist = sqrt(dx*dx + dy*dy + dz*dz);
		it++;
		if(it == plist.end()) break; // at last point
		it->getAnchor(x2, y2, z2);
		dx = x - x2; dy = y-y2, dz = z-z2;
		dist = dist + sqrt(dx*dx + dy*dy + dz*dz);
		if(dist < mindist){
			mindist = dist;
			mx1 = x1; my1 = y1; mz1 = z1;
			mx2 = x2; my2 = y2; mz2 = z2;
			insertpos = it;
			minFound = true;
		}
		it++;
	}
	// check the last segment
	it = plist.begin();
	it->getAnchor(x2, y2, z2);
	dx = x - x2; dy = y - y2, dz = z - z2;
	dist = dist + sqrt(dx*dx + dy*dy + dz*dz);
	if(dist < mindist){
		mindist = dist;
		mx1 = x1; my1 = y1; mz1 = z1;
		mx2 = x2; my2 = y2; mz2 = z2;
		minFound = true;
		insertAtEnd = true;
	}

	if(minFound){
		newbp.setAnchor(x, y, z);
		dx = mx1-x; dy = my1-y; dz = mz1-z;
		newbp.setControl2(x + 0.25*dx, y + 0.25*dy, z + 0.25*dz);
		dx = mx2-x; dy = my2-y; dz = mz2-z;
		newbp.setControl1(x + 0.25*dx, y + 0.25*dy, z + 0.25*dz);
		if(insertAtEnd){
			plist.push_back(newbp);
		}
		else{
			plist.insert(insertpos, newbp);
		}

		_numberOfSegments += 1;
	}
};


bool BezierCurve::selectAnchor(float x, float y, float z, int & anchorID)
{
	float x2, y2, z2, dx, dy, dz, mag;
	int id = 0;
	bool anchorFound = false;

	if(plist.empty()){
		anchorID = -1;
		return false;
	}

	// find first anchor within pickRadius
	list<BezierPoint>::iterator it = plist.begin();
	while(it != plist.end()){
		it->getAnchor(x2, y2, z2);
		dx = x - x2;
		dy = y - y2;
		dz = z - z2;
		mag = sqrt(dx*dx + dy*dy + dz*dz);
		//clog << "Mag = " << mag << endl;
		//clog << "(" << x << ", " << y << ", " << z << ")" << endl;
		//clog << "(" << x2 << ", " << y2 << ", " << z2 << ")" << endl;
		if(mag < pickRadius){
			anchorFound = true;
			anchorID = id;
			break;
		}
		id++;
		it++;
	}		

	return anchorFound;
};

bool BezierCurve::selectControl(float x, float y, float z, int anchorID, int & controlN)
{
	float x2, y2, z2, dx, dy, dz, mag;
	int id = 0;
	bool controlFound = false;

	if(plist.empty()){
		controlN = -1;
		return false;
	}

	list<BezierPoint>::iterator it = plist.begin();
	while((it != plist.end()) && (id != anchorID)){
		id++;
		it++;
	}

	//clog << "AnchorID = " << anchorID << " id = " << id << endl;

	// find (last) control within pickRadius
	it->getControl1(x2, y2, z2);
	dx = x - x2;
	dy = y - y2;
	dz = z - z2;
	mag = sqrt(dx*dx + dy*dy + dz*dz);
	//clog << "Mag = " << mag << endl;
	//clog << "(" << x << ", " << y << ", " << z << ")" << endl;
	//clog << "(" << x2 << ", " << y2 << ", " << z2 << ")" << endl;
	if(mag < pickRadius){
		controlFound = true;
		controlN = 1;
	}
	it->getControl2(x2, y2, z2);
	dx = x - x2;
	dy = y - y2;
	dz = z - z2;
	mag = sqrt(dx*dx + dy*dy + dz*dz);
	//clog << "Mag = " << mag << endl;
	//clog << "(" << x << ", " << y << ", " << z << ")" << endl;
	//clog << "(" << x2 << ", " << y2 << ", " << z2 << ")" << endl;
	if(mag < pickRadius){
		controlFound = true;
		controlN = 2;
	}

	return controlFound;
};


void BezierCurve::updateAnchor(float x, float y, float z, int anchorID)
{
	int id = 0;
	float xp, yp, zp, dx, dy, dz;

	list<BezierPoint>::iterator it = plist.begin();
	while((it != plist.end()) && (id != anchorID)){
		id++;
		it++;
	}

	if(it != plist.end()){
		it->getAnchor(xp, yp, zp);
		dx = x-xp; dy= y-yp; dz = z-zp; //compute offset
		it->setAnchor(x, y, z);
		// move control points in corresponding offset
		it->getControl1(xp, yp, zp);
		it->setControl1(xp+dx, yp+dy, zp+dz);
		it->getControl2(xp, yp, zp);
		it->setControl2(xp+dx, yp+dy, zp+dz);
	}
};

void BezierCurve::deleteAnchor(int anchorID)
{
	int id = 0;

	list<BezierPoint>::iterator it = plist.begin();
	while((it != plist.end()) && (id != anchorID)){
		id++;
		it++;
	}

	plist.remove(*it);

	_numberOfSegments -= 1;
};

void BezierCurve::updateControl(float x, float y, float z, int anchorID, int controlN)
{
	int id = 0;

	list<BezierPoint>::iterator it = plist.begin();
	while((it != plist.end()) && (id != anchorID)){
		id++;
		it++;
	}

	if(it != plist.end()){
		if(controlN == 1)
			it->setControl1(x, y, z);
		if(controlN == 2)
			it->setControl2(x, y, z);
	}
};

unsigned int BezierCurve::getNsegments()
{
	return _numberOfSegments;
};


void BezierCurve::getPoints(unsigned int segment, float* ctrlpoints )
{
	unsigned int segNum = 0;
	list<BezierPoint>::iterator it;

	assert(segment < plist.size());

	// jump to segment
	if(segment == 0){
		it = plist.begin();
		it->getAnchor(ctrlpoints[0], ctrlpoints[1], ctrlpoints[2]);
		it->getControl1(ctrlpoints[3], ctrlpoints[4], ctrlpoints[5]);
		it++;
		it->getControl2(ctrlpoints[6], ctrlpoints[7], ctrlpoints[8]);
		it->getAnchor(ctrlpoints[9], ctrlpoints[10], ctrlpoints[11]);
	}
	else if(segment == _numberOfSegments-1){
		it = --plist.end();
		if(!_closed)
			it--;
		it->getAnchor(ctrlpoints[0], ctrlpoints[1], ctrlpoints[2]);
		it->getControl1(ctrlpoints[3], ctrlpoints[4], ctrlpoints[5]);
		if(_closed)
			it = plist.begin();
		else
			it++;
		it->getControl2(ctrlpoints[6], ctrlpoints[7], ctrlpoints[8]);
		it->getAnchor(ctrlpoints[9], ctrlpoints[10], ctrlpoints[11]);
	}
	else{
		it = plist.begin();
		while(segNum != segment){
			it++;
			segNum++;
		}
		it->getAnchor(ctrlpoints[0], ctrlpoints[1], ctrlpoints[2]);
		it->getControl1(ctrlpoints[3], ctrlpoints[4], ctrlpoints[5]);
		it++;
		it->getControl2(ctrlpoints[6], ctrlpoints[7], ctrlpoints[8]);
		it->getAnchor(ctrlpoints[9], ctrlpoints[10], ctrlpoints[11]);
	}
};


bool BezierCurve::isEmpty()
{
	return plist.empty();
};


bool BezierCurve::isClosed()
{
	return _closed;
};

void BezierCurve::setMemento(const BezierCurveMemento& M)
{
	M.getState(_numberOfSegments, _closed, plist);
};


BezierCurveMemento* BezierCurve::createMemento()
{
	BezierCurveMemento* M = new BezierCurveMemento;

	M->setState(_numberOfSegments, _closed, plist);

	return M;
};
