/***************************************************************************
File: nrvector.hh
Abstract:  allocate a memory block for a vector
Created: 09/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:30 $
***************************************************************************/
#ifndef _NRVECTOR_
#define _NRVECTOR_

#include <stdlib.h>
#include "nrerror.h"

template <class T> T * nrvector(int nl,int nh);
/**
* A function template for allocate memory block for a vector
*   kernel code comes from 'Numeric Recipe'. It is modified as a template
*   for various data type.
* @param nl the start subscript for the vector
* @param nh the end subscript for the vector
* @return if success, the memory address for the allocated vector 
*/

template <class T>
T * nrvector(int nl,int nh)
{
        T *v;
        v=(T *)malloc((unsigned) (nh-nl+1)*sizeof(T));
        if (!v) nrerror("allocation failure in vector()");
        return v-nl;
}

#endif
