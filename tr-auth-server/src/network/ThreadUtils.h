/*
 *  ThreadUtils.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 02.06.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_THREADUTILS_H_
#define _TR_THREADUTILS_H_

#include <pthread.h>
#include "types.h"

extern pthread_mutex_t gMutex;
namespace Thread
{
	static unsigned int ThreadsCount = 1;
	static pthread_mutex_t* Mutex = &gMutex;
	void InitMutex(pthread_mutex_t* Mutex = Thread::Mutex);
	void DestroyMutex(pthread_mutex_t* Mutex = Thread::Mutex);
	void LockMutex(pthread_mutex_t* Mutex = Thread::Mutex);
	void UnlockMutex(pthread_mutex_t* Mutex = Thread::Mutex);
	
	bool New(pthread_t* id, void* (*function)(void*),void* Parameter);
	bool Cancel(pthread_t id);
	void Join(pthread_t id);
	void Exit();
	unsigned int  Count();
	void IncreaseCount();
	void DecreaseCount();
}

#endif