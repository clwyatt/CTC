/********************************************************
File: buildTube.hh
Abstract: Construnction functions for level set
Created: 11/21/2000 
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

#ifndef _BuildTube_hh
#define _BuildTube_hh 1

#include <list>

#include "LF3Dfull.hh"
#include "Vector.hh"
#include "Volume.hh"

#define REAL_FAR 1000000
#define FAR 999999

/**
 * Initialize the level function using an ellisoid
 * 
 * @param levelFunction the level function data
 * @param center the center location of the ellipsoid
 * @param scale the axis scaling of the ellipsoid
 * @param rotation the rotation of the ellipsoid around x, y, z
 */
void init_ellipsoid(LF3Dfull &levelFunction, IMTK::Vector<float> center, IMTK::Vector<float> scale, IMTK::Vector<float> rotation);

/**
 * Initialize the level function using a region growing procedure
 * 
 * @param levelFunction the level function data
 * @param seed the seed location of the region
 * @param thresh the threshold for the region
 * @param data the 3D image data
 */
void init_region(LF3Dfull &levelFunction, list< IMTK::Subscript<3> > & seeds, float thresh, const IMTK::Volume<unsigned short> & data);

/**
 * Initialize the level function using a region growing procedure on distance to estimated surface map
 * 
 * @param levelFunction the level function data
 * @param seed the seed location of the region
 * @param thresh the threshold for the region
 * @param data the 3D distance transform
 */
void init_region_dist(LF3Dfull &levelFunction, list< IMTK::Subscript<3> > & seeds, float thresh, const IMTK::Volume<unsigned short> & data);

/**
 * Rebuild the level function 
 * 
 * @param levelFunction the level function data (modified)
 */
void rebuildLevelFunction(LF3Dfull &levelFunction);


#endif
