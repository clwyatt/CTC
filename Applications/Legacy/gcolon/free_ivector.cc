#include "free_ivector.h"

void free_ivector(int * v,int nl,int nh)
/*  Frees an int vecotr allocated by ivector().  */
{	
	free((char*) (v+nl));
}
