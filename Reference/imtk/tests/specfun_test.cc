/********************************************************
File: specfun_test.cc
Abstract: Test file for Special functions. 
Created: 10/24/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1 $) 
by $Author: clwyatt $ 
on $Date: 2001/04/04 15:28:08 $
*********************************************************/
#include <iostream>

#include "SpecFun.hh"

using namespace std;

int main(int argc, char **argv) {

	cout << "erf(0.5) = " << IMTK::erf(0.5) << endl;
	cout << "erfinv(0.5) = " << IMTK::erfinv(0.5) << endl;
}
