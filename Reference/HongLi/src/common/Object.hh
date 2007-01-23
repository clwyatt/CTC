/********************************************************
File: Object.hh
Abstract: The base IMTK class. 
Created: 12/15/99
Author: Chris L. Wyatt

Last Revision ($Revision: 1.1.1.1 $) 
by $Author: hongli $ 
on $Date: 2002/12/06 21:49:33 $
*********************************************************/

//prevent recursive includes
#ifndef _Object_hh
#define _Object_hh 1

/** IMTK base class.

An abstract class from which all subsequent IMTK classes are derived.
Defines the internal reference counting variable and provides
methods for referencing and unreferencing an object.
The reference counting works as follows:
When an object is created, the refCount is moved to 1.
If code requires the use of an object it should first call reference()
use it, then call unreference();
When the refCount drops to zero, the object deletes itself.
A warning is printed to stdout if the object is explicitly deleted
and the reference count is not zero.

The motivation for this is that objects can be allocated, and should
self destruct when no more code needs it. That way, at least some of
the memory allocation takes care of itself.

This class should not be used directly.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.1.1.1 $
*/

namespace IMTK{

class Object
{
private:
	int refCount;
	
public:
	/// Default Constructor
	Object();
	
	/// Copy Constructor
	Object(const Object & O);
	
	/// Destructor
	~Object();
	
	/**
	 * Function to increase the reference count of this object
	 * 
	 */
	void addReference();
	
	/**
	 * Function to decrease the reference count.
	 * If the count drops to zero the object is automatically deleted.
	 * 
	 */
	void unReference();
};
}

#endif
