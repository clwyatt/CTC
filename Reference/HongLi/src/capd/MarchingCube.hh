/***************************************************************************
File: MarchingCube.hh
Abstract: MarchingCube class for marching cube algrithms
Created: 2002-06-24

(C) 2002 by Wake Forest University School of Medicine
                             and
        Virgina Polytechnic Institute and State University

Last Revision ($Revision: 1.1 $) by $Author: hongli $ on $Date: 2003/10/13 19:56:52 $
***************************************************************************/
#ifndef _MarchingCube_hh_
#define _MarchingCube_hh_


#include "Cube.hh"
#include "Triangle_with_Features.hh"
#include <slist>

class MarchingCube;

class IsoPoint
{
private: 
	Point p;
	MarchingCube * pCube;
	int vIndexin;
	int vIndexout;
	int faceIndex[2];
	bool mark; 
	int index;
public:
	IsoPoint(Point point) 
		{
		p = point;
		mark = false;
		index = vIndexin = vIndexout = -1;
		}
	IsoPoint(MarchingCube * cube, Point point) 
		{
		pCube = cube;
		p = point;
		index = vIndexin = vIndexout = -1;
		mark = false;
		
		}

	IsoPoint() 
		{
		index = vIndexin = vIndexout = -1;
		mark = false;
		}
		
	void setMark(bool value) {mark = value; }
	bool getMark(void) { return mark;}	
	void setIndex(int i) {index = i;}
	int getIndex(void) {return index;} 
	void setVIndexin(int index) { vIndexin = index;}
	void setVIndexout(int index) { vIndexout = index;}
	int getVIndexin() { return vIndexin;}
	int getVIndexout() {return  vIndexout;}
	void setFaceIndex(void);
	void getFaceIndex(int & f1, int & f2) 
		{
		f1 = faceIndex[0];
		f2 = faceIndex[1];
		 } 
	Point getPoint(void) { return p;}
};
	
	
	
class MarchingCube : public Cube
{
private :
	float threshold; /// the threshold for out and in
	std::slist<IsoPoint *> isoPoints;
	std::slist<Triangle *> triList; ///Triangle list
	bool flag;
	bool thrflag; // 0/1 high value in/out 
public:
	MarchingCube(mvVec3i iVertex) : Cube(iVertex) {}
	MarchingCube(Point Vertex) : Cube(Vertex) {}
	MarchingCube() : Cube() {}

	bool isoPointExist(void);
	std::slist<IsoPoint *> getIsoPoints(void);
	std::slist<Triangle *> getTriangles(void);
	void setThreshold(float t) {threshold = t;}
	float getThreshold(void) {return threshold;}
	void setFlag(bool v) {flag = v;}
	bool getFlag() {return flag;}
	void setThrFlag(bool v) {thrflag = v;}
	bool getThrFlag() {return thrflag;}
	IsoPoint * getNextIsoPoint(IsoPoint *curP, bool & dir);

}; 
	
#endif		
	
	
	
	
	
	
	
		  
