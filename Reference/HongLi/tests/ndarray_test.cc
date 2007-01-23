/********************************************************
File: ndarray_test.cc
Abstract: Test file for NDArray template. 
Created: 10/24/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/
#include <iostream>

#include "NDArray.hh"

using namespace std;

int main(int argc, char **argv) {

	IMTK::NDArray<double,3> array1;
	
	IMTK::Subscript<3> sub1;
	IMTK::Subscript<3> bounds;
	
	// bounds 
	bounds[0] = 2;
	bounds[1] = 3;
	bounds[2] = 3;

	array1.setBounds(bounds);
	
	array1.fill(6.5);

	IMTK::NDArray<double,3>::iterator it = array1.begin();
	
	while(it != array1.end()){
		cout << *it << endl;
		it++;
	}

	sub1[0] = 0;
	sub1[1] = 1;
	sub1[2] = 2;

	array1.set(sub1, 3.0);

	it = array1.begin();
	
	while(it != array1.end()){
		cout << *it << endl;
		it++;
	}
}
