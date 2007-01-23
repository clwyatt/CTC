/***************************************************************************
File: nrmatrix.hh
Abstract:  allocate a memory block for a matrix
Created: 09/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:32 $
***************************************************************************/

#ifndef _MATRIX_HH_
#define _MATRIX_HH_
#include <stdlib.h>
#include "nrerror.h"

template <class T>
T **matrix(int nrl,int nrh,int ncl,int nch);

/**
* A function template for allocate memory block for a matrix
*   kernel code comes from 'Numeric Recipe'. It is modified as a template
*   for various data type.
* @param nrl the start row subscript for the matrix
* @param nrh the end row subscript for the matrix
* @param ncl the start column subscript for the matrix
* @param nch the end column subscript for the matrix
* @return if success, the memory address for the allocated matrix 
*/
 
template <class T>
T **matrix(int nrl,int nrh,int ncl,int nch)
/*  Allocates a float matrix with range [nrl..nrh] [mcl..nch].  */
{
	int i;
	T **m;

/*  Allocate pointers to rows.  */
	m=(T  **) malloc((unsigned) (nrh-nrl+1)*sizeof(T*));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m -=nrl;

/*  Allocate rows and set pointers to them.  */
	for(i=nrl;i<=nrh;i++) {
		m[i]=(T *) malloc((unsigned) (nch-ncl+1)*sizeof (T));
		if (!m[i]) nrerror("allocation failure 2 in matrix()");
		m[i] -= ncl;
	}
	return m;
}	

#endif
