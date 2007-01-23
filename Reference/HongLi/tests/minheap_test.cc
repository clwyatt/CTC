/********************************************************
File: minheap_test.cc
Abstract: Test file for min heap template. 
Created: 01/11/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:27 $
*********************************************************/
#include <iostream>

#include "MinHeap.hh"

using namespace std;


// test object implementing compare method required by MinHeap
class testobj
{
private:
	int value;

public:

	testobj(){
		value = 0;
	}

	testobj(int theValue){
		value = theValue;
	}

	testobj(const testobj & TO){
		value = TO.value;
	}

	~testobj(){}

	bool compare(const testobj & arg){
		bool flg;

		flg = (value > arg.value);

		return flg;
	}

	int get(){
		return value;
	}

	void set(int theValue){
		value = theValue;
	}
};



int main(int argc, char **argv) {

	unsigned int i;
	testobj t1(-100), t2(-50), t3(50), t4(100);
	testobj temp, *testarray;
	unsigned int numarray;
	IMTK::MinHeap<testobj> minheap1(5000);
	IMTK::MinHeap<testobj> * minheap2;

	numarray = 10;
	testarray = new testobj[numarray];
	for(i = 0; i < numarray; i++){
		testarray[i].set(-i);
	}

	minheap1.make(testarray, numarray);

	minheap2 = new IMTK::MinHeap<testobj>(3000);
	minheap2->insert(t4);
	minheap2->insert(t2);
	minheap2->insert(t3);
	minheap2->insert(t1);

	while(!minheap1.isEmpty()){
		temp = minheap1.removeHeapRoot();
		cout << "Heap Root = " << temp.get() << endl;
	}

	cout << endl;
	while(!minheap2->isEmpty()){
		temp = minheap2->removeHeapRoot();
		cout << "Heap Root = " << temp.get() << endl;
	}

	minheap1.make(testarray, numarray);
	minheap1.insert(t4);
	minheap1.insert(t2);
	minheap1.insert(t3);
	minheap1.insert(t1);

	cout << endl;
	while(!minheap1.isEmpty()){
		temp = minheap1.removeHeapRoot();
		cout << "Heap Root = " << temp.get() << endl;
	}

	delete [] testarray;
	delete minheap2;
}
