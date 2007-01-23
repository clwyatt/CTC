/********************************************************
File: Thread.cc
Abstract: implementation for Thread class

See header (Thread.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:16 $
*********************************************************/

#include "Thread.hh"

using namespace IMTK;

Thread::Thread(){

	status = STOP;
}

Thread::Thread(const Thread & T){

}

Thread::~Thread(){

}

int Thread::run(){

	int error;

	error = 0;

	if(status != STOP){
		//IMTK::warn(IMTK_THREAD_RUNNING)
		// throw error ?

		return error;
	}

	error = pthread_create(&tid, NULL, Thread::cast_func, this);

	if(error){
		//IMTK::warn(IMTK_CREATE_THREAD_FAILED);
		// throw error ?
	}

	return error;
}

void * Thread::cast_func(void *thread){

	return static_cast<Thread *>(thread)->thread_func();
}

void Thread::suspend(){

	// down semaphore and block
	sem.down();
	status = SUSPEND;
}

void Thread::resume(){

	// up semaphore to continue
	status = RUN;
	sem.up();
}

void Thread::cancel(){

	sem.down();
	pthread_cancel(tid);
	status = CANCEL;
	sem.up();
	
}

void Thread::setStatus(enum Thread::IMTK_THREAD_STATUS theStatus){

	sem.down();
	status = theStatus;
	sem.up();
}

int Thread::getStatus(){

	IMTK_THREAD_STATUS temp;

	sem.down();
	temp = status;
	sem.up();

	return temp;

}

void Thread::join(){

	pthread_join(tid, NULL);
}
