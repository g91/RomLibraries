#include "stdafx.h"
#include "base_Thread.h"

#include "base_Thread_impl.h"


#include <assert.h>


//-----------------------------------------------------------------------------

namespace base
{


class ThreadImpl
{
public:
	THREAD_H	handle;
	Thread*		obj;
	bool		running;

	ThreadImpl()
	{
		handle	= 0;
		obj		= 0;
		running	= false;
	}

private:
	ThreadImpl( const ThreadImpl& );
	ThreadImpl& operator=( const ThreadImpl& );
};

//-----------------------------------------------------------------------------

static void* runThread( void* arg )
{
	ThreadImpl* thread = reinterpret_cast<ThreadImpl*>(arg);
	assert( thread->obj );
	assert( thread->running );

	try 
	{
		thread->obj->run();
	}
	catch ( ... )
	{
	}

	thread->running = false;
	thread->obj = 0;
	thread = 0;
	return 0;
}

//-----------------------------------------------------------------------------

Thread::Thread()
{
	m_this = new ThreadImpl;
}

Thread::~Thread()
{
	assert( !m_this->running );

	if ( m_this->handle )
	{
		Threadimp::detach( m_this->handle );
		m_this->handle = 0;
	}
}
bool	Thread::alive()
{
return m_this->running;

}
void Thread::start()
{
	assert( !m_this->running );

	if ( m_this->handle )
	{
		Threadimp::detach( m_this->handle );
		m_this->handle = 0;
	}

	m_this->obj = this;
	m_this->running = true;
	if ( Threadimp::create( &m_this->handle, runThread, m_this ) )
	{
		m_this->obj = 0;
		m_this->running = false;
		//throw Exception( Format("Failed to create a thread.") );
	}
}

void Thread::join()
{
	if ( m_this->handle )
	{
		Threadimp::join( m_this->handle );
		m_this->handle = 0;
	}
}

void Thread::run()
{
	assert( false );	// Thread is used by subclassing
}

void Thread::sleep( long millis )
{
	Threadimp::sleep( millis );
}


} // base
