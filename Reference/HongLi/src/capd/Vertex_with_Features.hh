/********************************************************
File: Vertex_with_Features.hh
Abstract:  A Vertex_withFeatures class derived from Vertex.
Created: 09/25/2001
Author: Hong Li

Last Revision  
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:31 $
*********************************************************/

#ifndef _Vertex_with_Features_hh
#define _Vertex_with_Features_hh 

#include <slist>


#include "Vertex.hh"
#include "mvVec3f.hh"

class Vertex_with_Features : public IMTK::Vertex
{

public : 

	///default constructor
	Vertex_with_Features() ;
	Vertex_with_Features(float thePX, float thePY, float thePZ, float theNX, float theNY, float theNZ)
	:Vertex(thePX, thePY, thePZ, theNX, theNY, theNZ) {};
	Vertex_with_Features( Vertex_with_Features &V);
	
	///deconstructor
	~Vertex_with_Features();
	
	void setIndex(int i) {index = i;}
	int getIndex() {return index;}
	
	///flags can be used for any purpose
	void setflag(bool f) {flag = f;}
	bool getflag() {return flag;}
	
	inline void setThickness(float a) { Thickness = a;}
	inline void setMeanCurvature(float a) { MeanCurvature = a;}
	inline void setGaussCurvature(float a) { GaussCurvature = a;}
	inline void setConvexity(float a) { Convexity = a;}
	inline void setNormDir(mvVec3f n) { NormDir = n;}
	inline float getMeanCurvature () { return MeanCurvature;}
	inline float getGaussCurvature () { return GaussCurvature;}
	inline float getThickness () { return Thickness;}
	inline float getConvexity () { return Convexity;}
	inline mvVec3f getNormDir () { return NormDir;}
	std::slist<int> getNeighborTriangleLabels() {return neighborTriangleLabels;}
	std::slist<void *> getNeighborTriangles() {return ptNeighborTriangles;}
	void addNeighborTriangleLabel(const int label) ;
	void addNeighborTriangle(void * pt) ;

	/**
	 * Present label comparison
	 *
	 */
	int sameAs(Vertex_with_Features & V){

		return (label() == V.label());
	}
	void setNormValid(bool a) {normValid = a;}
	bool isNormValid() {return normValid;}
	
	
private: 
	std::slist<int> neighborTriangleLabels;
	std::slist<void *> ptNeighborTriangles; 
	float Thickness;
	float MeanCurvature;
	float GaussCurvature;
	float Convexity;
	mvVec3f NormDir; 
	bool normValid;
	int index;
	bool flag; 
};

#endif
