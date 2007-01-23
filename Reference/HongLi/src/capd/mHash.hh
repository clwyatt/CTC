/********************************************************
File: mHash.hh
Abstract: An Hash container 
Created: 10/18/2002
Author: Hong Li

Last Revision ($Revision: 1.2 $) 
by $Author: hongli $ 
on $Date: 2002/12/19 22:24:50 $
*********************************************************/

//prevent recursive includes
#ifndef _mHash_hh
#define _mHash_hh 1


#include <math.h>
#include <mvVec3f.hh>
#include "Vertex_with_Features.hh"


typedef struct hashtable
{
//int index;
Vertex_with_Features * pV;
} HASHTABLE;

class mHash
{
public:
	mHash() {slotSize = 0;
		  tableSize = 0;};
	mHash(int tablesize, int slotsize, float x, float y, float z, float sx,
	float sy, float sz);
	~mHash();
	static double A;
	int slotSize; 
	int tableSize;


	HASHTABLE ** slots;
public: 	
	Vertex_with_Features * HashCompare(Vertex_with_Features * cur);
private:
	float dx,dy,dz,xo,yo,zo;	
};	

#endif
