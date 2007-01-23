#ifndef HOTELLING_HH_
#define HOTELLING_HH_
#include "mvVec2f.hh"
#include "mvVec3f.hh"
#include "nrmatrix.hh"
#include "nrfree_matrix.hh"
#include "nrfree_vector.hh"

 void hotelling(mvVec2f *pts, int npts, float *eigenvalues, float
**eigenvectors);

 void hotelling(mvVec3f *pts, int npts, float *eigenvalues, float
**eigenvectors);
#endif
