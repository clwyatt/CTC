/********************************************************
File: gridhash_test.cc
Abstract: Test file for GridHash template. 
Created: 01/25/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.3 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/31 00:42:54 $
*********************************************************/
#include <iostream>

#include "Volume.hh"
#include "GridHash.hh"

using namespace std;

int main(int argc, char **argv) {

	unsigned int key, index, count;
	bool fflag;
	IMTK::Volume<double> array1;
	IMTK::Subscript<3> bounds;
	IMTK::GridHash<unsigned int> hash(100);
	
	bounds[0] = 128;
	bounds[1] = 64;
	bounds[2] = 32;

	array1.setBounds(bounds);
	IMTK::NDArray<double,3>::iterator it = array1.begin();
	count = 0;
	while(it != array1.end()){
		*it = count;
		hash.insert(count+5, count);
		it++;
		count++;
	}

	key = 300;
	fflag = hash.find(key,index);
	if(fflag){
		cout << "Key " << key << " hashes to " << index << endl;
	}
	else{
		cout << "Hash Miss!" << endl;
	}

	key = 0;
	fflag = hash.find(key,index);
	if(fflag){
		cout << "Key " << key << " hashes to " << index << endl;
	}
	else{
		cout << "Hash Miss!" << endl;
	}

	key = 701;
	fflag = hash.find(key,index);
	if(fflag){
		cout << "Key " << key << " hashes to " << index << endl;
	}
	else{
		cout << "Hash Miss!" << endl;
	}

	key = 750;
	fflag = hash.find(key,index);
	if(fflag){
		cout << "Key " << key << " hashes to " << index << endl;
	}
	else{
		cout << "Hash Miss!" << endl;
	}

}




