/***************************************************************************
File: nrfree_vector.hh
Abstract:  Free the memory block for a vector allocated by nrvector
Created: 09/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:31 $
***************************************************************************/
#ifndef _FREE_VECTOR_HH_
#define _FREE_VECTOR_HH_
#include <stdlib.h>

template <class T>
void free_vector(T * v,int nl,int nh);


/**
* Free the memory block for a vector allocated by nrvector
* @param v the vector to be freed
* @param nl the start  subscript for the vector
* @param nh the end  subscript for the vector
*/

template <class T>
void free_vector(T * v,int nl,int nh)
/*  Frees an int vecotr allocated by nrvector().  */
{	
	free((char*) (v+nl));
}

#endif
