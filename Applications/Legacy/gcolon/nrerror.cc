#include "nrerror.h"

void nrerror(char error_text[])
{

	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n", error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}
