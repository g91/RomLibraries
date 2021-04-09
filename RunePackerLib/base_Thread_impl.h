#ifndef CAT3D_THEAD_IMPL_H
#define CAT3D_THEAD_IMPL_H


namespace base
{


/** 
 * Thread handle. 
 * 
 */
typedef struct _Thread_t {}*THREAD_H;

class Threadimp
{
public:
	/** 
	* Creates a new thread. 
	* 
	*/
	static int create( THREAD_H* t, void* (*start)(void*), void* arg );
	/** 
	* Marks thread resources for release. 
	*
	*/
	static int detach( THREAD_H t );
	/** 
	* Waits for thread to die and releases thread resources. 
	* 
	*/
	static int		join( THREAD_H t );

	/** 
	* Delays current thread for specified number of milliseconds. 
	* 
	*/
	static int		sleep( int millis );

};








} // base


#endif // CAT3D_THEAD_IMPL_H
