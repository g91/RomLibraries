#pragma once
#include <stddef.h>
#include <assert.h>
#include <memory.h>
#include <map>
#include "malloc/MyAlloc.h"
#pragma warning( disable:4267 )

namespace DTF
{
	//////////////////////////////////////////////////////////////////////////////////////////
	template< class T > 
	class MyVector
	{
	public:
		typedef T*	iterator;
	protected:
		T*				_allocData;
		int				_allocSize;
		int				_size;
		int				_blockSize;

	protected:
		void realloc( )
		{
			if( _allocSize > _size )
				return;

			int		newAllocSize = ( _allocSize + _allocSize /3 + 2 );
			char*	newAllocData = new char[ newAllocSize * _blockSize ];

			if( _allocData != NULL )
			{
				memcpy( newAllocData , _allocData , _allocSize * _blockSize );
				delete (char*)_allocData;
			}

			_allocData = (T*)newAllocData;
			_allocSize = newAllocSize;

		};


		void _SetMyVector( MyVector& in )
		{
			clear();
			_allocSize	= in._allocSize; 
			_size		= in._size;
			_blockSize	= in._blockSize;
			int dataSize = _allocSize * _blockSize;
			if ( dataSize > 0 )
			{
				_allocData	= (T*)new char[ dataSize ];
				memcpy( _allocData , in._allocData , dataSize );
			}
		}
	public:
		MyVector()
		{
			_allocData = NULL;
			_allocSize = 0;
			_size = 0;
			_blockSize = sizeof(T);
		}

		MyVector( MyVector& in )
		{
			_allocData = NULL;
			_allocSize = 0;
			_size = 0;
			_blockSize = sizeof(T);
			_SetMyVector( in );
		}

		~MyVector()
		{
			clear();
		}
		void operator = ( MyVector& in )
		{			
			_SetMyVector( in );
		}

		unsigned block_size()
		{
			return _blockSize;
		}

		T& operator[]( int i )
		{			
			return *(T*)((((char*)_allocData)+i*_blockSize));
		};

		void push_back( T& insertData )
		{
			realloc( );
			_allocData[ _size ] = insertData;
			_size++;
		}

		void push_back( void* insertData , int dataSize )
		{
			//assert( _blockSize == dataSize );
			if( dataSize > _blockSize )
				dataSize = _blockSize;

			realloc( );
			char* data = ( (char*)_allocData+ _size * _blockSize );			
			memcpy( data , insertData , dataSize );
			_size++;
		}

		void pop_back()
		{
			_size--;
		}

		void pop_front()
		{
			erase( 0 );
		}


		bool erase( const unsigned int id )
		{
			if( id >= (unsigned)_size )
				return false;

			memmove( &_allocData[ id  ]  , &_allocData[ id + 1  ] , (_size-id-1)*sizeof( T ) );
			_size--;
			return true;
		};

		void clear()
		{
			if( _allocData != NULL )
			{
				delete (char*)_allocData;
				_allocData = NULL;
			}
			_allocSize = 0;
			_size = 0;
		}

		T*	begin()
		{
			return _allocData;
		}

		T*	end()
		{
			return (_allocData + _size );
		}

		unsigned size()
		{
			return _size;
		}

	};
	//////////////////////////////////////////////////////////////////////////////////////////
	// µ¥¦P map<int,T*> 
	template< class T > 
	class MyMap
	{
	protected:
		std::map< int , T* >	_stdMap;
		int						_blockSize;
	public:
		typedef typename  std::map< int , T*>::iterator iterator;

		MyMap()
		{
			_blockSize = sizeof(T);
		}
		MyMap( MyMap& in )
		{
			_blockSize = sizeof(T);
			clear();
			iterator Iter;
			for( Iter = in.begin() ; Iter != in.end() ; Iter++ )
			{
				insert( Iter->first , (char*)Iter->second , _blockSize );
			}

		}

		~MyMap()
		{
			clear();
		}

		void operator = ( MyMap& in )
		{
			clear();
			iterator Iter;
			for( Iter = in.begin() ; Iter != in.end() ; Iter++ )
			{
				insert( Iter->first , (char*)Iter->second , _blockSize );
			}
		}

		T& operator[]( int Key )
		{					
			std::map< int , T*>::iterator Iter = _stdMap.find( Key );
			if( Iter == _stdMap.end() )
			{
				char* newData = NEW char[_blockSize];
				_stdMap[ Key ] = (T*)newData;

				return *(T*)newData;
			}

			return *(Iter->second);
		};

		void insert( int Key , char* Data , int Size )
		{
			if( Size > _blockSize )
				Size = _blockSize;

			std::map< int , T*>::iterator Iter = _stdMap.find( Key );
			if( Iter == _stdMap.end() )
			{
				char* newData = NEW char[_blockSize];
				_stdMap[ Key ] = (T*)newData;
				memcpy( newData , Data , Size );
			}
			else
			{
				memcpy( (void*)Iter->second , Data , Size );
			}
		}

		void clear()
		{
			std::map< int , T*>::iterator Iter;
			for( Iter = _stdMap.begin() ; Iter != _stdMap.end() ; Iter++ )
			{
				delete Iter->second;
			}
			_stdMap.clear();
		}

		iterator erase( int Key )
		{
			std::map< int , T*>::iterator Iter = _stdMap.find( Key );
			if( Iter == _stdMap.end() )
				return _stdMap.end();

			delete (T*)Iter->second;
			return _stdMap.erase( Iter );
		}

		iterator erase( iterator Iter )
		{
			delete (T*)Iter->second;
			return _stdMap.erase( Iter );
		}

		iterator begin()
		{
			return _stdMap.begin();
		}

		iterator end()
		{
			return _stdMap.end();
		}

		iterator find( int Key )
		{
			return _stdMap.find( Key );
		}

		unsigned size( )
		{
			return _stdMap.size();
		}

		unsigned block_size()
		{
			return _blockSize;
		}

	};
}