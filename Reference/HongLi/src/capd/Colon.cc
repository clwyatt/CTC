/********************************************************
File: Colon.cc
Abstract:  implementation of Colon.hh
Created: 04/17/2002
Author: Hong Li

Last Revision  
by $Author: hongli $ 
on $Date: 2003/10/02 19:44:19 $
*********************************************************/
#include <fstream>
#include <cstdio>
#include <list.h>
#include <stdlib.h>

#include "Colon.hh"
#include "CAPD_alg.h"
#include "constant.h"

/* group the lesion based on geometric features */
int Colon::Group_Lesion(const int16_t min_ithickness, const int32_t min_size, const int32_t min_area,
                               const int32_t use_area, const float min_conv)
{
  mvLesion *lesion;
//  float min_thickness;
  int32_t lesion_size;

cout << "in la "<<endl;

//  int32_t num_conv, num_thick;
  int16_t num_lesions = 0;
  int number = plumen->getNverticies();
  lesion = NULL;
if(number<=0) return -1;
  
Mesh_Features::vertexIterator mvi = plumen->vertexBegin();
plumen->setVerticesflag(false);
std::list<Vertex_with_Features *> vStack;
std::slist<Vertex_with_Features *> vnb ;
std::list<Vertex_with_Features *>::iterator nbiter; 
std::slist<Vertex_with_Features *>::iterator vnbiter;
//loop for all vertices
int i = 0;


//ofstream debugfile;

cout << "in la "<<endl;

while( mvi !=  plumen->vertexEnd() )
{
// If processed, continue.
    if((*mvi)->getflag())  
    	{
    	mvi++;
    	i++;
 	continue; 
        }

/*	cout<<"outer "<<(*mvi)->label()<<" M: "<<(*mvi)->getMeanCurvature()
		<<" G : "<<(*mvi)->getGaussCurvature()<<endl;
 	cout << (*mvi)->getPX()<<" "<<(*mvi)->getPY()<<" "
	         << (*mvi)->getPZ()<<endl;	
	if(i%100 == 0) getchar();*/	
 //If the current vertex is not a candidate    
    if(!isPolypCandidates(*mvi))	
    	{
    	(*mvi)->setflag(true);
	mvi++;
    	i++;
 	continue; 
        }
 	
        lesion_size = 0;  
    vStack.push_back(*mvi);
//while no vertex in the stack 
    nbiter = vStack.begin();
	
    while(nbiter != vStack.end())
	{
    // classify the vertex
	if(!isPolypCandidates(*nbiter))
		{
		(*nbiter++)->setflag(true);	
		continue;
		}
	
    //Process this vertex as an polyp vertex

    //new a mvLesion to start this one
        /* Vertex is part of a valid lesion.  Do a connected
         * vertices grow until all vertices in this lesion have been
         * identified and grouped.
         */

        if (lesion == NULL) {
		lesion = new mvLesion;
		assert(lesion);
		}
	
        lesion_size ++;
        lesion->vertexList.push_front(*nbiter);
    //neighborhood searching for all connect unormal vertex
        vnb = plumen->getNeighborVertices(*nbiter, 1, 2);
    //push all unprocessed neighbors to stack
	vnbiter = vnb.begin();
	while(vnbiter!=vnb.end())
	    {
	    if((*vnbiter)->label()!=(*nbiter)->label())
	    	vStack.push_back(*vnbiter);
	    vnbiter++;
	     }
	(*nbiter)->setflag(true);
	nbiter++;	
       }
//finished this lesion vertex selection
    vStack.clear();
//calculate the lesion features
    if(lesion_size > 5 && lesion_size < MAX_VERTEX_LESION)	
    {    
    	double isoPlanicity;
        lesion->area = plumen->getAreaFromVertexList(lesion->vertexList, isoPlanicity);
        lesion->isoplanicity = isoPlanicity;
        lesion->id = ++num_lesions;
	lesion->colon = (Colon *)this;
	lesion->setNormal();
	lesion->setCenter();
	lesion->setRadius();
	lesion->setVolumeSize();
		
        cout << "lesion " <<lesion->id<< ": size " <<lesion_size;
	cout <<", area "<<lesion->area;
	cout <<", min Radius "<<lesion->minRadius;
	cout <<", max Radius "<<lesion->maxRadius<<endl;
    
//add the lesion to list
        lesionList.push_front(lesion);
//delete this copy of lesion
        lesion=NULL;
    }    
    else
    {
//delete this copy of lesion
  //      cout << "lesion "<< lesion->id<<" too big or too small, size " <<lesion_size<<endl;
	delete lesion;
        lesion=NULL;
    }
    
    mvi++;
}
    cout <<"finished grouping" <<endl;

//finish grouping;

//clear flag
plumen->setVerticesflag(false);


//successfully grouped lesions 
	LesionGrouped = 1;
	return 1;
	
}

int Colon::Filter_Lesion()
{
//Successfully filtered lesions
  int16_t elongated, valid, filtered, onfold, oversize, undersize, spike, isolated;
  int n;
  std::slist<mvLesion *>::iterator lesion_iter = lesionList.begin(); 

  n = lesionList.size();

  if (n == 0) {
//    fprintf(stderr, "filter_Lesions: lesionList is empty.\n");
    return 0;
  }

  valid = 0;
  elongated = 0;
  filtered = 0;
  spike = 0;
  onfold = 0;
  undersize = 0;
  oversize = 0;
  isolated = 0;
  
  while(lesion_iter!=lesionList.end()){
	//add group filtering conditions here
      if ((*lesion_iter)->elongation() == 1) {
        (*lesion_iter)->filtered |= ELONGATED;
        elongated++;
      	}	

      if ((*lesion_iter)->undersize() == 1) {
        (*lesion_iter)->filtered |= UNDERSIZE;
        undersize++;
      	}	

      if ((*lesion_iter)->oversize() == 1) {
        (*lesion_iter)->filtered |= OVERSIZE;
        oversize++;
      	}	

      if ((*lesion_iter)->spike() == 1) {
        (*lesion_iter)->filtered |= SPIKE;
        spike++;
      	}	

      if ((*lesion_iter)->onfold() == 1) {
        (*lesion_iter)->filtered |= ONFOLD;
        onfold++;
      	}	

      if((*lesion_iter)->filtered == 0)
	{
	//clear flag
	//plumen->setVerticesflag(false);

      	if(plumen->isIsolate((*lesion_iter)->vCenter, 30.0f)){
        (*lesion_iter)->filtered |= ISOLATED;
        isolated++;
      }
 }     

      	
      if((*lesion_iter)->filtered == 0)
	  {
	 (*lesion_iter)->setBLength();
        	}	
valid++;
        
	cout << "lesion " <<(*lesion_iter)->id<< ": filter_flag "
	<<(*lesion_iter)->filtered <<endl;
	  
    	 lesion_iter++;
  }
 
    cout <<"finished filtering" <<endl;
 	LesionFiltered = 1;
	return 1;	
		
}
