/********************************************************
File: ragged_test.cc
Abstract: Test file for Ragged Array template. 
Created: 04/03/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1 $) 
by $Author: clwyatt $ 
on $Date: 2001/04/03 17:20:14 $
*********************************************************/
#include <iostream>

#include "RaggedArray.hh"

using namespace std;

int main(int argc, char **argv) {

	unsigned int r, c, counter;
	IMTK::RaggedArray<double> array1, array2(5);
	
	array1.appendRow(1, new double[1]);
	array1.appendRow(2, new double[2]);
	array1.appendRow(3, new double[3]);

	cout << array2.size() << " rows" << endl;
	array2.insertRow(0, 5, new double[5]);
	array2.insertRow(2, 4, new double[4]);
	array2.insertRow(3, 5, new double[5]);
	array2.insertRow(4, 4, new double[4]);
	cout << array2.size() << " rows" << endl;

	counter = 0;
	cout << array1.size() << " rows" << endl;
	for(r=0; r < array1.size(); r++){
		cout << array1.size(r) << " columns" << endl;
		for(c=0; c < array1.size(r); c++){
			array1[r][c] = (double)counter;
			counter++;
		}
	}
	cout << endl;

	for(r=0; r < array1.size(); r++){
		for(c=0; c < array1.size(r); c++){
			cout << array1[r][c] << " ";
		}
		cout << endl;
	}
	cout << endl;

	cout << array2.size() << " rows" << endl;
	for(r=0; r < array2.size(); r++){
		cout << array2.size(r) << " columns" << endl;
		for(c=0; c < array2.size(r); c++){
			array2[r][c] = (double)(r);
		}
	}
	for(r=0; r < array2.size(); r++){
		for(c=0; c < array2.size(r); c++){
			cout << array2[r][c] << " ";
		}
		cout << endl;
	}

}




