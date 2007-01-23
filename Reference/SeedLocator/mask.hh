/********************************************************
File: mask.hh
Abstract:
Created: 1999
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

#ifndef _Mask_hh
#define _Mask_hh 1

#include "Vector.hh"
#include "Volume.hh"

/**
 * Mask out the background and tissue
 * 
 */
void maskBg(IMTK::Volume<unsigned short> &data, IMTK::Volume<unsigned short> &region, float thresh);

/**
 * Mask out the region at given seed point
 * 
 */
void maskRegion(int sx, int sy, int sz, IMTK::Volume<unsigned short> &data);

#endif
