/*******************************************************
File: Semaphore.hh
Abstract: Semaphore class declaration 
Created: 11/15/99
Author: Chris Wyatt 

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:15 $
*******************************************************/

//prevent multiple includes
#ifndef _Semaphore_hh
#define _Semaphore_hh 1
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/** Class Semaphore.

An integer Semaphore class to allow controlled access to shared
resources between threads. Use Semaphore::up to release a resource
and Semaphore::down to lock a resource (blocks until available).

@author Christopher Lee Wyatt   <cwyatt@wfubmc.edu>
@version $Revision: 1.2 $
*/

namespace IMTK{

class Semaphore
{
private:
	int n;
	pthread_mutex_t mutex;
	pthread_cond_t cond;

public:
	/// default constuctor
	Semaphore(); 
	/// default destructor
	~Semaphore(); 
      
	/**
	 * Decrement Semaphore, blocks until resource is available
	 *
	 */
	int down();

	/**
	 * Increment Semaphore
	 *
	 */
	int up();

	/**
	 * Decrement Semaphore, does not block, use for init only
	 *
	 */
	int decrement();

};
}

#endif
