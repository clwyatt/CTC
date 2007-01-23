/********************************************************
File: Thread.hh
Abstract: The base thread class. 
Created: 12/29/1999
Author: Chris L. Wyatt

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:16 $
*********************************************************/

//prevent recursive includes

#ifndef _Thread_hh
#define _Thread_hh 1

#include <pthread.h>
#include "Object.hh"
#include "Semaphore.hh"

/** IMTK base thread class.

A class from which all subsequent threaded IMTK classes are derived.

Access to all data is controlled using an integer semaphore.

Data should be made available to the thread through the derived class
private or protected variables. i.e. the run methods takes no arguments.

@author Chris L. Wyatt <cwyatt@wfubmc.edu>
@version $Revision: 1.2 $
*/

namespace IMTK{

class Thread: public Object
{
public:
	enum IMTK_THREAD_STATUS {RUN, SUSPEND, CANCEL, STOP, FINISHED};

private:
	pthread_t tid;
	Semaphore sem;

protected:
	IMTK_THREAD_STATUS status;
	void setStatus(enum Thread::IMTK_THREAD_STATUS theStatus);

	/**
	 * Cast derived thread_func as static
	 * This makes sure this is a valid cast.
	 * 
	 */
	static void * cast_func(void *);

public:
	/// Default Constructor
	Thread();

	/// Copy Constructor
	Thread(const Thread & T);

	/// Destructor
	virtual ~Thread();

	/**
	 * Function to start the thread
	 * 
	 */
	int run();

	/**
	 * Virtual Function the thread executes
	 * 
	 */
	 virtual void * thread_func() = 0;

	/**
	 * Function to suspend the thread
	 * 
	 */
	void suspend();

	/**
	 * Function to continue thread execution after suspension
	 * 
	 */
	void resume();

	/**
	 * Function to cancel the thread
	 * 
	 */
	void cancel();

	/**
	 * Function to get the thread completion status
	 * 
	 */
	int getStatus();

	/**
	 * Function to join thread and wait for completion.
	 * NOTE: this blocks untill the thread completes.
	 * Use getStatus and compare to FINISHED if you want
	 * to test the state and continue.
	 * 
	 */
	void join();

};
}

#endif
