/********************************************************
File: filter.hh
Abstract: 3d filter routines
Created: 11/21/2000 
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

#ifndef _filter_hh
#define _filter_hh 1

#include "Volume.hh"

void gaussian_smooth(IMTK::Volume<unsigned short> &volume, float sigma);

#endif



