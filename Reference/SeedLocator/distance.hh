/********************************************************
File: distance.hh
Abstract:
Created: 1999
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

#ifndef _Distance_hh
#define _Distance_hh 1

#include "Volume.hh"

/**
 * 3-4-5 distance transform
 * 
 */
void computeDT(IMTK::Volume<unsigned short> &data);

#endif
