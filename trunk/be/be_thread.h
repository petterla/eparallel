/*
* Copyright (c) 2010
* efgod@126.com.
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation.  Silicon Graphics makes no
* representations about the suitability of this software for any
* purpose.  It is provided "as is" without express or implied warranty.
*/


#ifndef BE_THREAD_H
#define BE_THREAD_H



#ifdef _WIN32

#include <windows.h>


#else/*_WIN32*/

#include <pthread.h>
#include <semaphore.h>


#endif


namespace	be{

#ifdef	_WIN32
typedef	HANDLE				THREADHANDLE;
typedef	DWORD				PID;
typedef	CRITICAL_SECTION	MUTEX;
typedef	HANDLE				SEMAPHORE;
typedef	LPTHREAD_START_ROUTINE PTHREAD_FUNC;


#else
typedef	pthread_t			THREADHANDLE;
typedef	pid_t				PID;
typedef	pthread_mutex_t		MUTEX;
typedef	sem_t				SEMAPHORE;
typedef	void* (*PTHREAD_FUNC)(void*);

#endif

	int	be_thread_create	(THREADHANDLE*	thd, void* attr, PTHREAD_FUNC start_routine, void* arg);
	int	be_thread_join		(THREADHANDLE*	thd);
	#ifdef _WIN32
	int	be_thread_suspend	(THREADHANDLE*	thd);
	int	be_thread_resume	(THREADHANDLE*	thd);
	#endif
	int	be_mutex_init		(MUTEX*			mtx);
	int	be_mutex_take		(MUTEX*			mtx);
	int	be_mutex_give		(MUTEX*			mtx);
	int	be_mutex_destroy	(MUTEX*			mtx);
	int	be_sem_init			(SEMAPHORE*		sem, int initcnt,	int	maxcnt);
	int	be_sem_take			(SEMAPHORE*		sem);
	int	be_sem_try_take		(SEMAPHORE*		sem);
	int	be_sem_give			(SEMAPHORE*		sem);
	int	be_sem_destroy		(SEMAPHORE*		sem);
	int	be_sleep			(int			msec);

	class auto_lock
	{
	public:
		auto_lock(MUTEX*	lock):m_lock(lock){
			be::be_mutex_take(m_lock);
		}
		~auto_lock(	){
			be::be_mutex_give(m_lock);
		}
	protected:
	private:
		MUTEX*	m_lock;
	};

};

#endif/*BE_THREAD_H*/
