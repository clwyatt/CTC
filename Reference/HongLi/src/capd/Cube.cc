/***************************************************************************
File: Cube.cc
Abstract: implementation of Cube class
Created: 2002-08-24

(C) 2002 by Wake Forest University School of Medicine
                             and
        Virgina Polytechnic Institute and State University

Last Revision ($Revision: 1.2 $) by $Author: hongli $ on $Date: 2003/10/13 19:56:51 $
***************************************************************************/
#include "Cube.hh"
#include <stdlib.h> 
#include <math.h>

Cube::Cube()
{
	firstVertex = mvVec3f(0,0,0);
	for (int i=0;i<8; i++) values[i] = 0;
	xdim = 1;
	ydim = 1;
	zdim = 1;	
	}
	
Cube::Cube(Point Vertex)
{
	firstVertex = Vertex;
	for (int i=0;i<8; i++) values[i] = 0;
	xdim = 1;
	ydim = 1;
	zdim = 1;	
}

Cube::Cube(mvVec3i iVertex)
{
	firstVertex.x = (float)iVertex.x;
	firstVertex.y = (float)iVertex.y;
	firstVertex.z = (float)iVertex.z;
	for (int i=0;i<8; i++) values[i] = 0;
	xdim = 1;
	ydim = 1;
	zdim = 1;	
}

Point Cube::getPosition(int vIndex)
{
	switch (vIndex)
	{
	case 0:
		return firstVertex;
	case 1:
		return Point(firstVertex.x + xdim,  firstVertex.y, firstVertex.z);
	case 2:
		return Point(firstVertex.x + xdim,  firstVertex.y + ydim, firstVertex.z);
	case 3:
		return Point(firstVertex.x,  firstVertex.y + ydim, firstVertex.z);
	case 4:
		return Point(firstVertex.x,  firstVertex.y, firstVertex.z - zdim);
	case 5:
		return Point(firstVertex.x + xdim,  firstVertex.y, firstVertex.z - zdim);
	case 6:
		return Point(firstVertex.x + xdim,  firstVertex.y + ydim, firstVertex.z - zdim);
	case 7:
		return Point(firstVertex.x,  firstVertex.y + ydim, firstVertex.z - zdim);
	}
return Point(0,0,0);	
}

void Cube::setValue(int vIndex, float v)
{
values[vIndex] = v;
}

float Cube::getValue(int vIndex)
{
return values[vIndex];
}

bool Cube::isSameEdge(int v1, int v2)
{
	if(v1==4&&v2==3) return false;
	if(v1==3&&v2==4) return false;
	if(abs(v1-v2) == 1) return true;
	if(abs(v1-v2) == 4) return true;
	if(v1==0&&v2==3) return true;
	if(v1==3&&v2==0) return true;
	if(v1==4&&v2==7) return true;
	if(v1==7&&v2==4) return true;
	return false;
}

int Cube::face[6][4] = 
	{ 
	{0,1,2,3}, 
	{2,3,6,7},
	{4,5,6,7}, 
	{0,1,4,5}, 
	{0,3,4,7}, 
	{1,2,5,6}
	};	
	
int Cube::facenormal[6][2] = 
	{ 
	{0,4}, 
	{2,1},
	{4,0}, 
	{0,3}, 
	{0,1}, 
	{1,0}
	};	
	
		
