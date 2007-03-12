#include "matrix.h"

float **matrix(int nrl,int nrh,int ncl,int nch)
/*  Allocates a float matrix with range [nrl..nrh] [mcl..nch].  */
{
	int i;
	float **m;

/*  Allocate pointers to rows.  */
	m=(float  **) malloc((unsigned) (nrh-nrl+1)*sizeof(float*));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m -=nrl;

/*  Allocate rows and set pointers to them.  */
	for(i=nrl;i<=nrh;i++) {
		m[i]=(float *) malloc((unsigned) (nch-ncl+1)*sizeof (float));
		if (!m[i]) nrerror("allocation failure 2 in matrix()");
		m[i] -= ncl;
	}
	return m;
}	
