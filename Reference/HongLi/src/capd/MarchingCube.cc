/***************************************************************************
File: MarchingCube.cc
Abstract: MarchingCube class implementation for marching cube algrithms
Created: 2002-06-24

(C) 2002 by Wake Forest University School of Medicine
                             and
        Virgina Polytechnic Institute and State University

Last Revision ($Revision: 1.2 $) by $Author: hongli $ on $Date: 2003/10/13 19:56:52 $
***************************************************************************/
#include "MarchingCube.hh"
#include "Vertex_with_Features.hh"	
#include <stdio.h>

bool MarchingCube::isoPointExist(void)
{
int i,j ;
float a = 0;
	for(i=0;i<8;i++)
		{
		if((a = getValue(i) - threshold)!=0)
			break;
		}	 
	for(j = i; j < 8;j++)
	{
		if(a * (getValue(j) - threshold) <= 0)
			return true;
	}		 
return false;
}		
		
	

	
std::slist<IsoPoint *> MarchingCube::getIsoPoints(void)
{
	int i;
	int dir = 0;
	float v[8];
	float distance; 
	int index[2];
	Point p0;
	IsoPoint * isoPoint;
	int total = 0;
	for (i = 0 ; i < 8; i++)
		{
		v[i] = getValue(i) - threshold; 
		if(v[i]==0) v[i] = 0.0001;
//		cout << v[i] << " ";
		}
//		cout << endl;
	
	// go through all 12 edges on pCube
	for (i =0;i<12;i++) 
	{
	
	switch (i)
	{
		case 0: // the edge between vertex : 0-1
			index[0] = 0;
			index[1] = 1;
			dir = 0;  // x dir
			break;
		case 1: // the edge between vertex : 1-2
			index[0] = 1;
			index[1] = 2;
			dir = 1;  // y dir
			break;
		case 2: // the edge between vertex : 3-2
			index[0] = 3;
			index[1] = 2;
			dir = 0;
			break;
		case 3: // the edge between vertex : 3-0
			index[0] = 0;
			index[1] = 3;
			dir = 1;
			break;
		case 4: // the edge between vertex : 4-5
			index[0] = 4;
			index[1] = 5;
			dir = 0;
			break;
		case 5: // the edge between vertex : 5-6
			index[0] = 5;
			index[1] = 6;
			dir = 1;
			break;
		case 6: // the edge between vertex : 6-7
			index[0] = 7;
			index[1] = 6;
			dir = 0;
			break;
		case 7: // the edge between vertex : 7-4
			index[0] = 4;
			index[1] = 7;
			dir = 1;
			break;
		case 8: // the edge between vertex : 0-4
			index[0] = 4;
			index[1] = 0;
			dir = 2;			
			break;
		case 9: // the edge between vertex : 1-5
			index[0] = 5;
			index[1] = 1;
			dir = 2;
			break;
		case 10: // the edge between vertex : 2-6
			index[0] = 6;
			index[1] = 2;
			dir = 2;
			break;
		case 11: // the edge between vertex : 3-7
			index[0] = 7;
			index[1] = 3;
			dir = 2;
			break;
			}
			
			
	if(v[index[0]]*v[index[1]] < 0)
		{
		distance =fabs(v[index[0]]/(v[index[1]] - v[index[0]]));
		p0 = getPosition(index[0]);
		if(distance > 0.03 && distance < 0.97)
			{
			switch(dir)
				{
				case 0:
					p0.x = getXdim()*distance + p0.x;
					break;
				case 1:
					p0.y = getYdim()*distance + p0.y;
					break;
				case 2:
					p0.z = getZdim()*distance + p0.z;
					break;
				}
			}	
		else if(distance >=0.97)
			{
			p0 = getPosition(index[1]);
			}
			
		isoPoint = new IsoPoint(this,  p0);
		if(thrflag)
			{
			if (v[index[0]] > 0)
				{
				isoPoint->setVIndexin(index[0]);
				isoPoint->setVIndexout(index[1]);
				}	
			else 
				{
				isoPoint->setVIndexin(index[1]);
				isoPoint->setVIndexout(index[0]);
				}
			}
		else
			{
			if (v[index[0]] < 0)
				{
				isoPoint->setVIndexin(index[0]);
				isoPoint->setVIndexout(index[1]);
				}	
			else 
				{
				isoPoint->setVIndexin(index[1]);
				isoPoint->setVIndexout(index[0]);
				}
			}
			
		isoPoint->setFaceIndex();
		isoPoint->setIndex(total ++);	
		isoPoints.push_front(isoPoint);
//		cout << "iso index"<<isoPoint->getVIndexin() <<" "<< isoPoint->getVIndexout() <<endl; 
		}				

	}
	setFlag(true);	 /// set flag that the isopoints are ready
	return isoPoints;
}


std::slist<Triangle *> MarchingCube::getTriangles(void)
{

std::slist<IsoPoint *>::iterator  pIsoIter;
Point first_point, cur_point;
Vertex_with_Features * pV[3], * temp;
Triangle *tri;
bool dir; //dir
int num; 
IsoPoint * pIso, *pIso0;

if(!getFlag())
	getIsoPoints();
	
pIsoIter = isoPoints.begin();
//cout << isoPoints.size() << endl;
while(pIsoIter != isoPoints.end())
	{
	if ((*pIsoIter)->getMark())
		{
		pIsoIter++;
		continue;
		}
	first_point = cur_point = (*pIsoIter)->getPoint();
	num = 0;
	pV[num] = new Vertex_with_Features(cur_point.x, cur_point.y, cur_point.z,0,0,0);
	pIso0 = pIso = *pIsoIter; 
	pIso->setMark(true);
	num = num+1;	 	
	while((pIso = getNextIsoPoint(pIso, dir))!= NULL) 
		{
		cur_point =  pIso->getPoint();
		pIso->setMark(true);

		//check if the secoind point is a duplicate
		if(cur_point == first_point) 
			continue;
		
		pV[num] = new Vertex_with_Features(cur_point.x, cur_point.y, cur_point.z,0,0,0);

		//check the direction here
		if(!dir)
			{
			pIso = pIso0; 
			temp = pV[0];
			pV[0] = pV[1];
			pV[1] = temp;
			}			
		num = num + 1;
		break;
		}
	if(pIso == NULL)	 
		{
		if(pV[0]) delete pV[0];
			return triList;
		} 
	while((pIso = getNextIsoPoint(pIso, dir))!= NULL) 
		{
			cur_point = pIso->getPoint();
			pIso->setMark(true);
			
			pV[num] = new Vertex_with_Features(cur_point.x, cur_point.y, cur_point.z,0,0,0);			
			if(*(pV[num]) == *(pV[0]) || *(pV[num]) == *(pV[1])) 
				{
				delete pV[num];
				continue;
				}
				
			
			tri = new Triangle(pV[0], pV[1], pV[2]);
				
			/*cout << tri->getV1()->getPX()
			<< tri->getV1()->getPY()
			<< tri->getV1()->getPZ()<<endl;
			cout << tri->getV2()->getPX()
			<< tri->getV2()->getPY()
			<< tri->getV2()->getPZ()<<endl;
			cout << tri->getV3()->getPX()
			<< tri->getV3()->getPY()
			<< tri->getV3()->getPZ()<<endl;
			cout <<"================"<<endl;	
			getchar();*/
			triList.push_front(tri);
			pV[1] = pV[2];
		}
	pIsoIter ++;	
	}
//cout << triList.size() <<endl;	
return triList;	
}
	
IsoPoint * MarchingCube::getNextIsoPoint(IsoPoint *curP, bool &dir)
{
int i;
bool gotNext;
std::slist<IsoPoint *>::iterator  pIsoIter;
pIsoIter = isoPoints.begin();
int f1, f2, pf1, pf2;
Point a, b, c; 
curP->getFaceIndex(f1, f2);
int curin = curP->getVIndexin();
int curout = curP->getVIndexout();
int pin, pout;
	while(pIsoIter != isoPoints.end())
		{
		gotNext = false;
		if ((*pIsoIter)->getMark() || (*pIsoIter)->getIndex() == curP->getIndex())
			{
			pIsoIter++;
			continue;
			}
		
		/// Find co face point
		(*pIsoIter)->getFaceIndex(pf1,pf2);
		pin = (*pIsoIter)->getVIndexin();
		pout = (*pIsoIter)->getVIndexout();
		if(pf1 == f1 || pf2 == f1 || pf1 == f2 || pf2 == f2)
		{	
			int f = 0;
			if(pf1 == f1) f = f1;
			else if(pf1 == f2) f = f2;
			else if(pf2 == f1) f = f1;
			else if(pf2 == f2) f = f2;
			c = getPosition(facenormal[f][0]) - getPosition(facenormal[f][1]);
			if(curin == pin)
				{
				 a = getPosition(curout) - getPosition(curin);
				 b = getPosition(pout )- getPosition(pin);
				if (c.innerProduct(a.crossProduct(b)) > 0) dir = true;
				else dir = false; 				
				return (*pIsoIter);
				}
			else 
			   {
			   if(isSameEdge(curin, pin))
				{
				 b = getPosition(pout) - getPosition(curin);
				 a = getPosition(curout) - getPosition(curin);
				if (c.innerProduct(a.crossProduct(b)) > 0) dir = true;
				else dir = false; 				
				return (*pIsoIter);
				}
			   else {
				for(i = 0; i<4; i++)
					{	
					if((face[f][i] != curin)&&(face[f][i] != curout)&&
						(face[f][i] != pin)&&(face[f][i] != pout)) 
					 	break;
					}
					if(i!=4) 
					{
						if(!thrflag && getValue(face[f][i]) < threshold)
							{
							gotNext = true;
				 			a = getPosition(curout) - getPosition(curin);
				 			b = getPosition(pout) -getPosition( pin);
							}
						if(thrflag && getValue(face[f][i]) > threshold)
							{
				 			a = getPosition(curout) - getPosition(curin);
				 			b = getPosition(pout) - getPosition(pin);
							gotNext = true;
							}
						if(gotNext)	
							{
							if (c.innerProduct(a.crossProduct(b)) > 0) dir = false;
							else dir = true; 				
							return (*pIsoIter);
							}
					}		
				}
			    }	
		}
		pIsoIter++;					
	}
return NULL;				  
}		
		

void IsoPoint::setFaceIndex(void)
{
int v[2];

v[0] = getVIndexin();
v[1] = getVIndexout();


//cout << v[0] <<" --- "<< v[1] <<endl;
if(v[0]==-1)
	return;

if ((v[0] == 0 && v[1]== 1) || (v[0] == 1 && v[1]== 0))
	{
	faceIndex[0] = 0;
	faceIndex[1] = 3;
	return;
	}
	 

if ((v[0] == 2 && v[1]== 1) || (v[0] == 1 && v[1]== 2))
	{
	faceIndex[0] = 0;
	faceIndex[1] = 5;
	return;
	}
	
if ((v[0] == 2 && v[1]== 3) || (v[0] == 3 && v[1]== 2))
	{
	faceIndex[0] = 0;
	faceIndex[1] = 1;
	return;
	}
	
if ((v[0] == 0 && v[1]== 3) || (v[0] == 3 && v[1]== 0))
	{
	faceIndex[0] = 0;
	faceIndex[1] = 4;
	return;
	}
	
if ((v[0] == 4 && v[1]== 5) || (v[0] == 5 && v[1]== 4))
	{
	faceIndex[0] = 2;
	faceIndex[1] = 3;
	return;
	}
	
if ((v[0] == 5 && v[1]== 6) || (v[0] == 6 && v[1]== 5))
	{
	faceIndex[0] = 2;
	faceIndex[1] = 5;
	return;
	}
	
if ((v[0] == 6 && v[1]== 7) || (v[0] == 7 && v[1]== 6))
	{
	faceIndex[0] = 1;
	faceIndex[1] = 2;
	return;
	}
	
if ((v[0] == 7 && v[1]== 4) || (v[0] == 4 && v[1]== 7))
	{
	faceIndex[0] = 2;
	faceIndex[1] = 4;
	return;
	}
	
if ((v[0] == 0 && v[1]== 4) || (v[0] == 4 && v[1]== 0))
	{
	faceIndex[0] = 3;
	faceIndex[1] = 4;
	return;
	}
	
if ((v[0] == 3 && v[1]== 7) || (v[0] == 7 && v[1]== 3))
	{
	faceIndex[0] = 1;
	faceIndex[1] = 4;
	return;
	}
	
if ((v[0] == 2 && v[1]== 6) || (v[0] == 6 && v[1]== 2))
	{
	faceIndex[0] = 1;
	faceIndex[1] = 5;
	return;
	}
	
if ((v[0] == 5 && v[1]== 1) || (v[0] == 1 && v[1]== 5))
	{
	faceIndex[0] = 3;
	faceIndex[1] = 5;
	return;
	}
}			
		


	
	
	
	
	
	
	
		  
