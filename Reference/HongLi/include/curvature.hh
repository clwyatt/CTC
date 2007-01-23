/***************************************************************************
File: curvature.hh
Abstract:  header for curvature.cc
Created: 09/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.2 $) 
by $Author: hongli $ on $Date: 2003/10/02 19:45:40 $
***************************************************************************/

#ifndef _Curvature_h
#define _Curvature_h 


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "nrfree_matrix.hh"
#include "nrinvert.hh"
#include "nrmatrix.hh"
#include "nrsvbksb.hh"
#include "nrsvdcmp.hh"
#include "Vertex_with_Features.hh"
#include "mvVec3f.hh"
#include "Volume_ext.hh"

#define GAUSS 0
#define MEAN  1

/**
* Calculate the curvature for a given vertex in a vertice's list
* @param vert the vertex to be calculated
* @param nblist the vertice's list
* @param cond condition number of the intermediate matrix to be inverted
* @param DB debug flag
* @param volume_ext the volume data for the surface
* @param g the obtained Gaussian curvature
* @param m the obtained Mean curvature
* @return 1 success, 0 for fail
*/ 
int curvature(Vertex_with_Features & vert, std::slist<Vertex_with_Features *>
nblist, double &cond, int DB, Volume_ext<unsigned short> & volume_ext, float &g,
float & m);

/**
* Calculate the curvature for a given vertex in a vertice's list
* @param vert the vertex to be calculated
* @param nblist the vertice's list
* @param g the obtained Gaussian curvature
* @param m the obtained Mean curvature
* @return 1 success, 0 for fail
*/ 
int curvature(Vertex_with_Features & vert, std::slist<Vertex_with_Features *>
nblist, float &g,
float & m);

/**
* Calculate the curvature for a given vertex in a vertice's list
* @param vert the vertex to be calculated
* @param nblist the vertice's list
* @param g the obtained Gaussian curvature
* @param m the obtained Mean curvature
* @param maxPrinciple the obtained  maximum principle curvature
* @param minPrinciple the obtained  minimum principle curvature
*
* @return 1 success, 0 for fail
*/ 

int curvature(Vertex_with_Features & vert, std::slist<Vertex_with_Features *>
nblist, mvVec3f norm, float &g, float & m, float & maxPrinciple, float
 & minPrinciple, double & a, double & b, double & c, double & Ay, double & Az,double * trans );
 
double gxangle(mvVec3f p);
double gyangle(mvVec3f p);
double gzangle(mvVec3f p);

#endif
