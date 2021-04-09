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

#include <stdlib.h>
#include <string.h>

#include "buffer.hpp"

curlpp::buffer::buffer() :
      m_buffer( NULL ),
      m_length( 0 )
{}

curlpp::buffer::buffer( const char *data, size_t length ) : m_buffer( NULL ), m_length( 0 )
{
   set( data, length );
}

curlpp::buffer::buffer( const buffer& other ) : m_buffer( NULL ), m_length( 0 )
{
   set( other.m_buffer, other.m_length );
}

curlpp::buffer::~buffer()
{
   if ( m_buffer != NULL )
   {
      free( ( void* ) m_buffer );
   }
}

const char *
curlpp::buffer::get() const
{
   return m_buffer;
}

size_t 
curlpp::buffer::length() const
{
   return m_length;
}

void 
curlpp::buffer::set( const char *buffer, size_t length )
{
   reset();

   if ( buffer != NULL )
   {
      if ( ( m_buffer = ( char* ) malloc( length + 1 ) ) != NULL )
      {
         m_length = length;
         memcpy( m_buffer, buffer, m_length );
         m_buffer[ m_length ] = 0;
      }
   }
}

void 
curlpp::buffer::reset()
{
   m_length = 0;

   if ( m_buffer != NULL )
   {
      free( ( void* ) m_buffer );
      m_buffer = NULL;
   }
}

bool 
curlpp::buffer::is_valid() const
{
   return m_buffer != NULL;
}

