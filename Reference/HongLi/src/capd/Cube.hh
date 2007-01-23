/***************************************************************************
File: Cube.hh
Abstract: Cube class for marching cube algrithms
Created: 2002-06-24

(C) 2002 by Wake Forest University School of Medicine
                             and
        Virgina Polytechnic Institute and State University

Last Revision ($Revision: 1.1 $) by $Author: hongli $ on $Date: 2003/10/13 19:56:51 $
***************************************************************************/
#ifndef _Cube_hh_
#define _Cube_hh_

#include "mvVec3f.hh"
#include "mvVec3i.hh"

typedef mvVec3f Point;


/*********************************************************/
/************* Vertex Order  and coordinat direction *****/
/*


          7___________6             + Y              
         /|          /|             |                
        / |   B     / |             |                                                                  
      3/__|_______2/  |             |                     
       |  |     C  |  |             |__________+ X               
       |E |        |F |            /                 
       |  |4_A_____|__|5          /                      
       | /         | /           /                     
       |/     D    |/           /                
      0/__________1/           + Z

************************************************************/












class Cube 
{
private :
	float xdim;
	float ydim;
	float zdim;
	Point firstVertex; ///the lower left corner
	float values[8]; /// the value on the 8 vetex
public:
	static int face[6][4];			
	static int facenormal[6][2];			
	Cube();
	Cube(Point vertex);
	Cube(mvVec3i iVertex);
	Point getPosition(int vIndex);
	void setValue(int vIndex, float value);
	float getValue(int vIndex);
	void setXdim(float dim) {xdim = dim;}
	void setYdim(float dim) {ydim = dim;}
	void setZdim(float dim) {zdim = dim;}
	float getXdim(void) {return xdim;}
	float getYdim(void) {return ydim;}
	float getZdim(void) {return zdim;}
	bool isSameEdge(int v1, int v2);
	
};
	 	
#endif
