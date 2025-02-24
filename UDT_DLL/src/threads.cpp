#include "threads.hpp"
#include "thread_local_allocators.hpp"
#include "memory.hpp"


#if defined(UDT_WINDOWS)
#	include <Windows.h>
#else
#	include <pthread.h>
#	include <stdlib.h>
#	include <string.h>
#endif


#if defined(UDT_WINDOWS)

DWORD WINAPI GlobalThreadCallback(LPVOID lpThreadParameter)
{
	udtThread* const thread = (udtThread*)lpThreadParameter;
	if(thread == NULL)
	{
		return 1;
	}

	thread->InvokeUserFunction();

	udtThreadLocalAllocators::ReleaseThreadLocalAllocators();

	return 0;
}

#else

void* GlobalThreadCallback(void* threadParameter)
{
	udtThread* const thread = (udtThread*)threadParameter;
	if(thread == NULL)
	{
		return (void*)1;
	}

	thread->InvokeUserFunction();

	udtThreadLocalAllocators::ReleaseThreadLocalAllocators();

	return (void*)0;
}

#endif


udtThread::udtThread()
{
	_threadhandle = NULL;
	_userData = NULL;
	_entryPoint = NULL;
}

udtThread::~udtThread()
{
	Release();
}

bool udtThread::CreateAndStart(ThreadEntryPoint entryPoint, void* userData)
{
	if(_threadhandle != NULL)
	{
		return false;
	}

	_entryPoint = entryPoint;
	_userData = userData;

#if defined(UDT_WINDOWS)

	const HANDLE thread = CreateThread(NULL, 0, &GlobalThreadCallback, this, 0, NULL);
	_threadhandle = thread;

	return thread != NULL;

#else

	pthread_t thread;
	pthread_attr_t attribs;
	pthread_attr_init(&attribs);
	pthread_attr_setdetachstate(&attribs, PTHREAD_CREATE_JOINABLE);
	const bool success = pthread_create(&thread, &attribs, &GlobalThreadCallback, this) == 0;
	pthread_attr_destroy(&attribs);
	if(success)
	{
		_threadhandle = (u8*)udt_malloc(sizeof(pthread_t));
		memcpy(_threadhandle, &thread, sizeof(pthread_t));
	}

	return success;

#endif
}

bool udtThread::Join()
{
	if(_threadhandle == NULL)
	{
		return false;
	}
	
#if defined(UDT_WINDOWS)
	return WaitForSingleObject((HANDLE)_threadhandle, INFINITE) == WAIT_OBJECT_0;
#else
	return pthread_join(*(pthread_t*)_threadhandle, NULL) == 0;
#endif
}

struct args {
	int joined;
	pthread_t td;
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	void **res;
};

static void *waiter(void *ap)
{
	struct args *args = ap;
	pthread_join(args->td, args->res);
	pthread_mutex_lock(&args->mtx);
	args->joined = 1;
	pthread_mutex_unlock(&args->mtx);
	pthread_cond_signal(&args->cond);
	return 0;
}

#if defined(__aarch64__)
int pthread_timedjoin_np(pthread_t td, void **res, struct timespec *ts)
{
	pthread_t tmp;
	int ret;
	struct args args = { .td = td, .res = res };

	pthread_mutex_init(&args.mtx, 0);
	pthread_cond_init(&args.cond, 0);
	pthread_mutex_lock(&args.mtx);

	ret = pthread_create(&tmp, 0, waiter, &args);
	if (!ret)
		do ret = pthread_cond_timedwait(&args.cond, &args.mtx, ts);
		while (!args.joined && ret != ETIMEDOUT);

	pthread_mutex_unlock(&args.mtx);

	pthread_cancel(tmp);
	pthread_join(tmp, 0);

	pthread_cond_destroy(&args.cond);
	pthread_mutex_destroy(&args.mtx);

	return args.joined ? 0 : ret;
}
#endif

bool udtThread::TimedJoin(u32 timeoutMs)
{
	if(_threadhandle == NULL)
	{
		return false;
	}

#if defined(UDT_WINDOWS)

	return WaitForSingleObject((HANDLE)_threadhandle, (DWORD)timeoutMs) == WAIT_OBJECT_0;

#elif defined(_GNU_SOURCE) || defined(__aarch64__)

	timespec ts;
	if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		return false;
	}
	ts.tv_nsec += (long)timeoutMs * (long)1000000;

	return pthread_timedjoin_np(*(pthread_t*)_threadhandle, NULL, &ts) == 0;
#else

#	error "pthread_timedjoin_np doesn't exist on your platform"

#endif
}

void udtThread::Release()
{
	if(_threadhandle != NULL)
	{
#if defined(UDT_WINDOWS)
		CloseHandle((HANDLE)_threadhandle);
#else
		free(_threadhandle);
#endif

		_threadhandle = NULL;
	}
}

void udtThread::InvokeUserFunction()
{
	if(_entryPoint != NULL)
	{
		(*_entryPoint)(_userData);
	}
}
