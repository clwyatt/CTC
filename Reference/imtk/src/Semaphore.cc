/********************************************************
File: Semaphore.cc
Abstract: implementation for Semaphore class to facilitate 
          shared thread resources

See header (Semaphore.hh) for documentation

Last Revision ($Revision: 1.2 $) 
by $Author: clwyatt $ 
on $Date: 2001/03/28 19:56:15 $
*********************************************************/

#include "Semaphore.hh"

using namespace IMTK;

Semaphore::Semaphore(){

	n = 1;

	if(pthread_mutex_init(&mutex, NULL) == -1){
		fprintf(stderr, "Error creating Semaphore mutex.\n");
		exit(1);
	}

	if(pthread_cond_init(&cond, NULL) == -1){
		fprintf(stderr, "Error creating Semaphore condition signal.\n");
		exit(1);
	}
}

Semaphore::~Semaphore(){

	if(pthread_mutex_destroy(&mutex) == -1){
		fprintf(stderr, "Error destroying Semaphore mutex.\n");
		exit(1);
	}

	if(pthread_cond_destroy(&cond) == -1){
		fprintf(stderr, "Error destroying Semaphore condition signal.\n");
		exit(1);
	}
}

int Semaphore::down(){

	int after;

	pthread_mutex_lock(&mutex);

	while(n <= 0)
		pthread_cond_wait(&cond, &mutex);

	n--;
	after = n;

	pthread_mutex_unlock(&mutex);

	return after;
	
}

int Semaphore::up(){

	int after;

	pthread_mutex_lock(&mutex);
	n++;
	after = n;

	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);

	return after;
}

int Semaphore::decrement(){

	int after;

	pthread_mutex_lock(&mutex);
	n--;
	after = n;
	pthread_mutex_unlock(&mutex);

	return after;
}
