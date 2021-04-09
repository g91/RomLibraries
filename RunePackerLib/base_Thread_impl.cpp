#include "stdafx.h"
#include "base_Thread_impl.h"
//#include "base_Debug.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <process.h>
	#include <stdio.h>
#else
	#include <unistd.h>
	#include <pthread.h>
#endif

//#include <config.h>

//-----------------------------------------------------------------------------

namespace base
{


typedef void* (*start)(void*);

struct ThreadParam
{
	start	func;
	void*	arg;
};

//-----------------------------------------------------------------------------

#ifdef _WIN32
static unsigned __stdcall threadProc( void* a )
{
	ThreadParam*	param	= (ThreadParam*)a;
	start			func	= param->func;
	void*			arg		= param->arg;

	delete param;
	param = 0;

	unsigned rv = (unsigned)func( arg );
	_endthreadex( rv );
	return rv;
}
#endif

//-----------------------------------------------------------------------------

int Threadimp::create( THREAD_H* t, void* (*start)(void*), void* arg )
{
#ifdef _WIN32

	ThreadParam* param = new ThreadParam;
	param->func = start;
	param->arg = arg;
	unsigned id;
	HANDLE h = (HANDLE)_beginthreadex( 0, 0, threadProc, param, 0, &id );
	if ( 0 == h )
	{
		*t = 0;
		delete param;
		return 1;
	}
	else
	{
		*t = (THREAD_H)h;
		//Debug::printf( _SC("The thread %X (handle=%X}) started."), (int)id, (int)h );
		return 0;
	}

#else

	// pthread_t is not a ptr in every platform
	pthread_t* h = new THREAD_H;
	int err = pthread_create( h, 0, start, arg );
	if ( err ) 
	{
		delete h;
		*t = 0;
	}
	else
	{
		*t = (Thread_t)h;
	}
	return err;

#endif
}

int Threadimp::detach( THREAD_H t )
{
#ifdef _WIN32

	HANDLE h = (HANDLE)t;
	if ( h )
	{
	//	Debug::printf( _SC("Detaching thread (handle=%X)."), (int)h );
		CloseHandle( h );
		return 0;
	}
	return 1;

#else

	pthread_t* h = (pthread_t*)t;
	int rv = pthread_detach( *h );
	delete h;
	return rv;

#endif
}

int Threadimp::join( THREAD_H t )
{
#ifdef _WIN32

	HANDLE h = (HANDLE)t;
	if ( h )
	{
	//	Debug::printf(_SC( "Joining thread (handle=%X)..."), (int)h );
		DWORD rv = WaitForSingleObject( h, INFINITE );
		if ( WAIT_OBJECT_0 == rv )
		{
			CloseHandle( h );
			return 0;
		}
	}
	return 1;

#else

	pthread_t* h = (pthread_t*)t;
	int rv = pthread_join( h );
	delete h;
	return rv;

#endif
}

int Threadimp::sleep( int millis )
{
#ifdef _WIN32

	Sleep( millis );
	return 0;

#else

	usleep( millis*1000 );
	return 0;

#endif
}


} // base
