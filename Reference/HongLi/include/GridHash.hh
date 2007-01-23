/********************************************************
File: GridHash.hh
Abstract: A hash table for NDarray grid locations. 
Created: 01/24/2001
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:28 $
*********************************************************/

//prevent recursive includes
#ifndef _Grid_Hash_hh
#define _Grid_Hash_hh 1

#ifdef IRIX
#include <assert.h>
#include <math.h>
#else
#include <cassert>
#include <cmath>
#endif

#include <slist>

#include "Data.hh"

#define DEFAULT_HASH_TABLE_SIZE 31981

/**  A hash table for NDarray grid locations

A chained hash table for mapping a grid index to an associated
data object. The template is parameterized by the data
object associated with the index.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

template <class T>
class GridHash: public Data 
{
private:

	struct HashEntry{
		T data;
		unsigned int key;
	};

	std::slist<HashEntry> *hashTable;
	unsigned int hashTableSize;

protected:

	/**
	 * Compute the Hashing Function. This implementation
	 * uses a simple mod operator to do the hashing.
	 * 
	 */
	unsigned int hashFunc(unsigned int key){
		return (int)fmod((float)(key), (float)(hashTableSize));
	}


public:

	/// Default Constructor
	GridHash(){

		hashTableSize = DEFAULT_HASH_TABLE_SIZE;
		hashTable = new std::slist<HashEntry>[hashTableSize];
	}

	/**
	 * Construct hash table of given size 
	 * 
	 * @param theSize the size of the hash table to create
	 */
	GridHash(unsigned int theSize){

		assert(theSize > 0); // not usefull (and complicates things) to create a zero size table

		hashTableSize = theSize;
		hashTable = new std::slist<HashEntry>[hashTableSize];
	}

	/// Copy Constructor
	GridHash(const GridHash<T> & GH){

		delete [] hashTable;

		hashTableSize = GH.hashTableSize;
		hashTable = new std::slist<HashEntry>[hashTableSize];

		for(int i=0; i < hashTableSize; i++){
			hashTable[i] = MH.hashTable[i];
		}
	}

	/// Destructor
	~GridHash(){

		delete [] hashTable;
	}

	/**
	 * Function to insert into hash table
	 * 
	 * @param theData object to insert
	 * @param theKey the hash key for the object
	 */
	void insert(const T & theData, unsigned int theKey){

		unsigned int hashIndex;
		HashEntry entry;

		hashIndex = hashFunc(theKey);

		entry.key = theKey;
		entry.data = theData;

		hashTable[hashIndex].push_front(entry);
	}

	/**
	 * Function to insert into hash table
	 * 
	 * @param theKey the hash key for the object
	 * @param theData reference to the data structure to fill
	 *
	 * @return bool indicating found (true) or not found (false)
	 */
	bool find(const unsigned int theKey, T & theData){

		unsigned int hashIndex;
		bool found = false;

		hashIndex = hashFunc(theKey);

		if(!hashTable[hashIndex].empty()){

			std::slist<HashEntry>::iterator it = hashTable[hashIndex].begin();
			while(it != hashTable[hashIndex].end()){
				if( it->key == theKey ){
					found = true;
					theData = it->data;
					break;
				}
				
				it++;
			}
		}

		return found;
	}
};
}

#endif
