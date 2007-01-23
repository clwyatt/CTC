/********************************************************
File: Data.hh
Abstract: The base IMTK data class. 
Created: 01/12/2000
Author: Chris L. Wyatt

Last Revision ($Revision: 1.2 $) 
by $Author: hongli $ 
on $Date: 2003/10/02 19:45:39 $
*********************************************************/

//prevent recursive includes

#ifndef _Data_hh
#define _Data_hh 1

#include <typeinfo>

#include "Object.hh"

/** IMTK base data class.

An abstract class from which all subsequent IMTK data classes are derived.

Provides the ability to check if the structure type of the pointed to object is
the same as a specifyed type. This is usefull for generic overloading of operations 
like +, -, *, etc.

It also provides a read/write lock interface (using integer Semaphore) for threaded apps.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.2 $
*/
namespace IMTK{

class Data: public Object
{
private:

public:
	/// Default Constructor
	Data(){
	};

	/// Copy Constructor
	Data(const Data & D): Object(){
	};

	/// Destructor
	virtual ~Data(){
	};


	/**
	 * Check if an Object derived from Data is 
	 * of the the same class as the caller.
	 * i.e. do they have the same structure.
	 *
	 * @param theObject a pointer to the object in question
	 * @return bool indicating yes (true) or no (false) 
	 */
	bool isSameStructure(Data * theObject){
		bool status;

		if( typeid(*this) == typeid(*theObject) )
			status = true;
		else
			status = false;

		return status;
	};

};
}

#endif
