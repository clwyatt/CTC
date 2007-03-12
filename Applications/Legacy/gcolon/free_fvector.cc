#include "free_fvector.h"

void free_fvector(float * v,int nl,int nh)
/*  Frees an float vector allocated by fvector().  */
{	
	free((char*) (v+nl));
}
