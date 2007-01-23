/********************************************************
File: image_test.cc
Abstract: Test file for Image template. 
Created: 11/04/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/31 00:42:54 $
*********************************************************/
#include <iostream>

#include "Image.hh"

using namespace std;

int main(int argc, char **argv) {

	unsigned int count;
	IMTK::Image<double> array1;
	IMTK::Subscript<2> sub1;
	IMTK::Subscript<2> bounds;
	
	bounds[0] = 5;
	bounds[1] = 10;

	array1.setBounds(bounds);
	IMTK::NDArray<double,2>::iterator it = array1.begin();
	count = 0;
	while(it != array1.end()){
		*it = count;
		it++;
		count++;
	}

	cout << "Value at (3,3) = " << array1.getPixel(3, 3) << endl;

	sub1[0] = 3;
	sub1[1] = 3;

	cout << "Value at (3,3) = " << array1.get(sub1) << endl;
 
}




