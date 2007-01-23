/***************************************************************************
File: nrfree_matrix.hh
Abstract:  Free the memory block for a matrix allocated by nrmatrix
Created: 09/2001

(C) 2002 by Wake Forest University School of Medicine

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ on $Date: 2002/12/06 21:49:31 $
***************************************************************************/

#ifndef _FREE_MATRIX_HH_
#define _FREE_MATRIX_HH_

#include <stdlib.h>

template <class T>
void free_matrix(T ** m,int nrl,int nrh,int ncl,int nch);

/**
* Free the memory block for a matrix allocated by nrmatrix
* @param m the matrix to be freed
* @param nrl the start row subscript for the matrix
* @param nrh the end row subscript for the matrix
* @param ncl the start column subscript for the matrix
* @param nch the end column subscript for the matrix
*/
   
template <class T>
void free_matrix(T ** m,int nrl,int nrh,int ncl,int nch)

{
	int i;
	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}

#endif
