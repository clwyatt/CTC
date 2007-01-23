/********************************************************
File: MinHeap.hh
Abstract:  A minimum heap implementation.
Created: 12/15/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

//prevent recursive includes

#ifndef _MinHeap_hh
#define _MinHeap_hh 1

#ifdef IRIX
#include <assert.h>
#else
#include <cassert>
#endif

#include "Data.hh"

#define DEFAULT_HEAP_SIZE 100

/** Minimum Heap template.

Implements minimum heap binary tree with property
every node is equal to or greater than its parent.

The template is paramterized by the object type T.
The object T must provide a method compare(const T & arg) that
returns true if the argument is operationally greater
than the object instance.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

template <class T>
class MinHeap: public Data
{
private:
	
	T * heapArray;
	unsigned int currentHeapSize; // size of max heap + 1 (for 1... indexing)
	unsigned int maxHeapSize;

protected:


public:
	/// Default Constructor
	MinHeap(){

		maxHeapSize = DEFAULT_HEAP_SIZE;
		heapArray = new T[maxHeapSize + 1];
		currentHeapSize = 0;
	}

	/**
	 * Construct min heap of given size 
	 * 
	 * @param theSize the size of the heap to create
	 */
	MinHeap(unsigned int theSize){

		assert(theSize > 0); // not usefull (and complicates things) to create a zero size heap

		maxHeapSize = theSize;
		heapArray = new T[maxHeapSize + 1];
		currentHeapSize = 0;
	}

	/// Copy Constructor
	MinHeap(const MinHeap<T> & MH){

		delete [] heapArray;

		maxHeapSize = MH.maxHeapSize;
		currentHeapSize = MH.currentHeapSize;
		heapArray = new T[maxHeapSize + 1];

		for(int i=0; i < currentHeapSize; i++){
			heapArray[i] = MH.heapArray[i];
		}
	}

	/// Destructor
	~MinHeap(){

	}

	/**
	 * Function to make a heap from scratch
	 * This destroys the current contents of the heap
	 * 
	 * @param theArray pointer to an array
	 * @param theSize the size of the array
	 */
	void make(T * theArray, unsigned int theSize){

		assert(theSize > 0); // not usefull (and complicates things) to create a zero size heap

		// if array longer than current max size, automatically expand
		if(theSize > maxHeapSize){
			delete [] heapArray;

			maxHeapSize = theSize;
			heapArray = new T[maxHeapSize + 1];

			for(unsigned int i=0; i < theSize; i++){
				insert(theArray[i]);
			}
		}
		else{
			for(unsigned int i=0; i < theSize; i++){
				insert(theArray[i]);
			}
		}
	}

	/**
	 * Function to insert into Heap
	 * 
	 * @param theData reference to object to insert
	 */
	unsigned int insert(const T & theData){

		unsigned int i, j;

		currentHeapSize += 1;
		assert(currentHeapSize <= maxHeapSize);

		i = currentHeapSize;
		j = i >> 1;

		// find the proper position for insertion
		while( (j >= 1) && (heapArray[j].compare(theData)) ){
			
			heapArray[i] = heapArray[j]; // move parent down one position
			i = j;
			j >>= 1;
		}

		// copy to location found above
		heapArray[i] = theData;

		return i;
	}


	/**
	 * Function to adjust the Heap after removing the root
	 * 
	 */
	void adjust(unsigned int validHeapIndex){
		
		unsigned int i, j;
		bool found;
		T temp;
		
		i = validHeapIndex;
		temp = heapArray[i]; // temp holder for node to be dropped through the tree
		found = false;	
		j = i << 1;

		while( (j <= currentHeapSize) && (!found) ){
			// move to right child depending on comparision
			if( (j < currentHeapSize)  && heapArray[j].compare(heapArray[j+1]) ){
				j = j + 1;
			}

			if(!temp.compare(heapArray[j])){
				found = true;
			}
			else{
				// move child up
				heapArray[j >> 1] = heapArray[j];
				
				// move to next position
				j <<= 1;
			}
		}

		// place the node
		heapArray[j >>= 1] = temp;
	}

	/**
	 * Overload [] operator
	 * 
	 * @param theI
	 */
	inline T& operator[](unsigned int theI){

		assert(theI <= currentHeapSize);
		return heapArray[theI]; 
	}

	/**
	 * Function to remove Heap root and readjust heap
	 * 
	 * @return a copy of the root
	 */
	T removeHeapRoot(){

		T temp;

		assert(currentHeapSize > 0);

		temp = heapArray[1];

		heapArray[1] = heapArray[currentHeapSize];
		currentHeapSize -= 1;

		adjust(1);

		return temp;
	}


	/**
	 * return bool indicating if heap is empty
	 *
	 */
	bool isEmpty(){

		return (bool)(currentHeapSize == 0);
	}
};
}

#endif
