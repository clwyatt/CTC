/***************************************************************************
File: mvLesion.hh
Abstract:  Lesion groups in Computer Assisted Polyp Detection (CAPD)
Created: 03/2002

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.4 $) 
by $Author: hongli $ on $Date: 2003/10/02 19:44:22 $
***************************************************************************/

#ifndef _mvLesion_hh_
#define _mvLesion_hh_
#include <fstream>
#include <cstdio>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <constant.h>
#include "Vertex_with_Features.hh"
class mvLesion;
#include "Colon.hh"
#include "nrjacobi.hh"

/**
* a class for lesion
*/ 
class mvLesion {
  public:
	/// the colon where the lesion comes from
    Colon * colon;
    ///lesion id
    int16_t id;
    ///lesion rank
    int16_t rank;
    /// a list of vertices that represent the lesion on the surface
    std::slist<Vertex_with_Features *> vertexList;
    /// filtering flag 
    int filtered;
//    mvVertex center;
//    mvIndex centerIndex;
//    mvVertex centroid;
//    mvNormal normal;

	///normal direction of the lesion
    mvVec3f normal;
    mvVec3f center;
    
    /// center vertex of the lesion
    Vertex_with_Features * vCenter;	
    
    float rating;
    float ratio;
    float maxRadius;
    float minRadius;
    float height;
    float convexity;
    float curvature;
    float area;
    float blength; //boundary length
    float thickness;
    float max_thickness;
    float volumeSize;
    float isoplanicity;
    
    ///criteria
    float min_area; 
    float max_radius;
    float size_ratio;
    float min_radius;
    float max_ratio;
    float min_elongation;

/// fitting surface and Geomatric Transformation Matrix
    double a,b,c;
    double Ay, Az;
    double TranX[16]; /* 4 by 4*/
  public:
  
  /// a constructor
    mvLesion(void) {
      colon=NULL;	
      id = -1;
      rank = -1;
      filtered = 0;
 //     centerIndex = -1;
      rating = 0.0f;
      height = 0.0f;
      convexity = 0.0f;
      curvature = 0.0f;
      area = 0.0f;
      blength = 0.0f;
      volumeSize = 0.0f;
      isoplanicity = 0.0f;
      ratio = 0;
      
      //initialize the criteria
     min_area = MIN_AREA; 
     size_ratio = SIZE_RATIO;
     max_radius = MAX_RADIUS;
     min_radius = MIN_RADIUS;
     max_ratio = MAX_RATIO;
     min_elongation = MIN_ELONGATION;

    }
	///a destructor
    ~mvLesion(void) {
      id = -1;
//      filtered = FILTER_NONE;
    }
    
    	///test the lesion is too small or not
    int undersize()
    	{
	if(area < min_area) return true;
	//if(area < 100) return area < 0.9 * Pi * maxRadius * minRadius;
	return 0;
	}
	
    	///test the lesion is too big wrt the radius or not
    int oversize()
    	{
	return area > Pi * maxRadius * minRadius * size_ratio;
	}
	
    	///test the lesion is a noise or not
    int spike()
    	{
	if(fabs(minRadius) < min_radius || fabs(maxRadius) < min_radius)
		return true;
	else 
		return false;
	}
		
    	///test the lesion is on fold or not
    int onfold()
    	{
	if(fabs(minRadius)/fabs(maxRadius) > max_ratio || fabs(minRadius) > max_radius)
		return true;
		
	if(isoplanicity * area < 2.84) return true; 
	
	return false;
	}	
    	///test the lesion is elongated or not
    int elongation();
    
    	///output the lesion to oogl file
    void write_oogl(float r, float g, float b);
    	///Calculate the normal direction
    void setBLength();
    void setNormal();
    	///Calculate the center vertex  
    void setCenter();
    	///Calculate the Volume size the lesion covers  
    void setVolumeSize();
    	///Calculate the lesion radius
    void setRadius();	
    	///Set up the detection criteria
    void setCriteria(float ma, float sizer, float maxr, float minr, float maxratio, float minelg)
    	{
    	if( ma != -1) min_area = ma;
    	if( sizer != -1) size_ratio = sizer;
    	if( maxr != -1) max_radius = maxr;
    	if( minr != -1) min_radius = minr;
    	if( maxratio != -1) max_ratio = maxratio;
    	if( minelg != -1) min_elongation = minelg;
	}
    	
};

#endif
