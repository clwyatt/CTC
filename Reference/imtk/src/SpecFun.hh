/********************************************************
File: SpecFun.hh
Abstract: Computational routines for special functions.
          Most of these routines follow NRC, except for
	  erfinv, which is similar to the method used in 
	  Octave and Matlab.

Created: 12/29/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1 $) 
by $Author: clwyatt $ 
on $Date: 2001/04/04 15:28:07 $
*********************************************************/

//prevent recursive includes

#ifndef _SpecFun_hh
#define _SpecFun_hh 1

namespace IMTK{

/**
* Compute the error function
* 
* @param x the input value
* @return result
*/
double erf(double x);

/**
* Compute the inverse error function
* 
* @param x the input value
* @return result
*/
double erfinv(double x);

/**
* Compute the incomplete gamma function, P(a,x)
* 
* @param x the input value
* @return result
*/
double gammap(double a, double x);


}

#endif
