/********************************************************
File: Capd_alg.c
Abstract:  the static class for CAPD algorithms include
Created: 04/17/2002
Author: Hong Li

Last Revision  
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:30 $
*********************************************************/
#include "Vertex_with_Features.hh"
#include <cstdio>

#define  MIN_CONVEXITY -0.5
#define  MAX_MEAN_CURVATURE -0.1
#define  MIN_MEAN_CURVATURE 0.0

#define  MIN_GAUSS_CURVATURE 0.0
bool isPolypCandidates(Vertex_with_Features *ptVert)
{
/* Thickness will be implemented later */
		
     float mc = ptVert->getMeanCurvature();
     float gc = ptVert->getGaussCurvature();		
    if (mc > MAX_MEAN_CURVATURE) return false;
    if (gc < MIN_GAUSS_CURVATURE) return false;
    
    
  /*  float r = mc + (5.0/3.0)*gc +0.2;
    if (r < 0) return false;
    */
    
    return true;
}    
   
    
