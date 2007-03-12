#include "free_matrix.h"

void free_matrix(float ** m,int nrl,int nrh,int ncl,int nch)

{
	int i;
	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}
