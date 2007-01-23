/********************************************************
File: tetradecomp.hh
Abstract: isosurface extraction routines
Created: 11/21/2000 
Author: Chris L. Wyatt

Last Revision ($Revision$) by $Author$ on $Date$
*********************************************************/

#ifndef _tetradecomp_hh
#define _tetradecomp_hh 1

#include "LF3Dfull.hh"
#include "Mesh.hh"

IMTK::Mesh * tetradecomp(LF3Dfull &lf, int method);

void ooglwrite(IMTK::Mesh *mesh, char * name);

#endif
