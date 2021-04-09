/*
*    Copyright (c) <2002-2005> <Jean-Philippe Barrette-LaPierre>
*    
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files 
*    (curlpp), to deal in the Software without restriction, 
*    including without limitation the rights to use, copy, modify, merge,
*    publish, distribute, sublicense, and/or sell copies of the Software,
*    and to permit persons to whom the Software is furnished to do so, 
*    subject to the following conditions:
*    
*    The above copyright notice and this permission notice shall be included
*    in all copies or substantial portions of the Software.
*    
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
*    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
*    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "curlpp.hpp"

using namespace curlpp;

memory_trait::memory_trait() :
      m_buffer( NULL ),
      m_length( 0 ),
      m_size( 0 ),
      m_pos( 0 ),
      m_zero_pos( ( size_t ) - 1 ),
      m_own( true )
{}


memory_trait::memory_trait( void *buffer, size_t length, bool own ) :
      m_buffer( buffer ),
      m_length( length ),
      m_size( length ),
      m_pos( 0 ),
      m_zero_pos( ( size_t ) - 1 ),
      m_own( own )
{}


memory_trait::~memory_trait()
{
   if ( m_own && ( m_buffer != NULL ) )
      free( m_buffer );
}


void memory_trait::reset()
{
   if ( m_own && ( m_buffer != NULL ) )
      free( m_buffer );
      
   m_buffer = NULL;
   
   m_own = true;
   
   m_length = m_size = m_pos = 0;
   
   m_zero_pos = ( size_t ) - 1;
}


bool memory_trait::mrealloc( size_t length )
{
   void * thebuffer;
   size_t theSize = ( ( size_t ) ( ( ( m_length + length ) / 2048 ) + 1 ) ) * 2048;
   
   if ( m_buffer == 0 )
   {
      if ( ( thebuffer = malloc( theSize ) ) == NULL )
         return false;
   }
   
   else
      if ( ( thebuffer = realloc( m_buffer, theSize ) ) == NULL )
         return false;
         
   m_buffer = thebuffer;
   
   m_size = theSize;
   
   return true;
}


size_t memory_trait::write( void *buffer, size_t length )
{
   if ( ( m_length + length ) > m_size )
      if ( !mrealloc( length ) )
         return 0;
         
   memcpy( ( char* ) m_buffer + m_length, buffer, length );
   
   m_length += length;
   
   return length;
}


size_t memory_trait::read( void *buffer, size_t length )
{
   if ( m_buffer == NULL )
      return 0;
      
   if ( ( length + m_pos ) > m_length )
      length = ( m_length - m_pos );
      
   memcpy( buffer, ( char* ) m_buffer + m_pos, length );
   
   m_pos += length;
   
   return length;
}


const char *memory_trait::string()
{
   char theZero = 0;
   
   // Add a terminating zero if not already done
   
   if ( m_zero_pos != m_length )
   {
      if ( !write( &theZero, 1 ) )
         return ( char* ) NULL;
         
      m_zero_pos = --m_length;
   }
   
   return ( char* ) m_buffer;
}

