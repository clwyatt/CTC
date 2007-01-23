/********************************************************
File: levelSetEvolve.hh
Abstract: evolution of GDM
Created: 11/21/2000 
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

#ifndef _levelSetEvolve_hh
#define _levelSetEvolve_hh 1


#include "Volume.hh"
#include "LF3Dfull.hh"

void levelSetEvolve(LF3Dfull &levelFunction,
		    IMTK::Volume<float> &cmap,
		    int numIterations, 
		    double timeStep, 
		    double epsilon,
		    double F0);


#endif
