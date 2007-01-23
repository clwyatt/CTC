/********************************************************
File: vector_test.cc
Abstract: Test file for Vector template. 
Created: 11/04/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/
#include <iostream>

#include "Vector.hh"

using namespace std;

int main(int argc, char **argv) {

	unsigned int count;
	IMTK::Vector<double> array1, array2;
	IMTK::Subscript<1> sub1;
	IMTK::Subscript<1> bounds;
	
	bounds[0] = 10;

	array1.setBounds(bounds);
	array2.setBounds(bounds);

	IMTK::NDArray<double,1>::iterator it = array1.begin();
	count = 0;
	while(it != array1.end()){
		*it = count;
		array2[count] = count;
		it++;
		count++;
	}

	cout << "Array1 value at (3) = " << array1.getElement(3) << endl;

	sub1[0] = 3;

	cout << "Array1 value at (3) = " << array1.get(sub1) << endl;

	cout << "Array1 value at (9) = " << array1.getElement(9) << endl;
	cout << "Array2 value at (9) = " << array2[9] << endl;
 
}




